#include "VulkanDeviceMemoryStore.h"

using namespace EngineAPI::Graphics::Platform;

bool VulkanDeviceMemoryStore::InitVKDeviceMemoryStore(EngineAPI::Graphics::DeviceMemoryAllocator* parentAllocator,
	VkDevice* logicalDevice,
	VkDeviceSize deviceMemorySizeInBytesToAlloc,
	VkPhysicalDeviceMemoryProperties* fullDeviceMemoryProperties,
	uint32_t memoryTypeIndex, bool isPublicMemoryStore)
{
	//Cache the owning allocator (incase we go down the route of
	//multiple allocators!)
	parentMemoryAllocator = parentAllocator;

	//Cache the logical device
	cachedVkLogicalDevice = *logicalDevice;

	//Cache the memory property flag for future use
	vkMemoryPropertyFlags = fullDeviceMemoryProperties->memoryTypes[memoryTypeIndex].propertyFlags;

	//Are we requesting more bytes than is available?
	uint32_t heapIndex = fullDeviceMemoryProperties->memoryTypes[memoryTypeIndex].heapIndex;
	VkDeviceSize maxHeapSize = fullDeviceMemoryProperties->memoryHeaps[heapIndex].size;
	if (deviceMemorySizeInBytesToAlloc > maxHeapSize)
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryBlock::InitVKDeviceMemoryBlock() - Requested memory block size is too large for this device!\n");
		return false;
	}

	//Check if memory is  mappable
	bool isMemoryMappable = false;
	if (vkMemoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
		isMemoryMappable = true;

	//Alloc info struct
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.allocationSize = deviceMemorySizeInBytesToAlloc;
	allocInfo.memoryTypeIndex = memoryTypeIndex;

	//Alloc a chunk of memory for this store.
	VkResult result = vkAllocateMemory(cachedVkLogicalDevice, &allocInfo, nullptr, &vkMemoryStoreHandle);
	if (result != VK_SUCCESS)
	{
		//Error:
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryBlock::InitVKDeviceMemoryBlock() - Error allocating device memory block.\n");
		return false;
	}

	//Cache data RE this store
	memoryStoreSizeBytes = allocInfo.allocationSize;
	vkMemoryTypeIndex = memoryTypeIndex;
	vkIsStoreMemoryMappable = isMemoryMappable;
	isPublicStore = isPublicMemoryStore;

	//Get the host memory pointer if available
	if (isMemoryMappable)
	{
		VkResult result = vkMapMemory(cachedVkLogicalDevice, vkMemoryStoreHandle, 0, memoryStoreSizeBytes, VK_WHOLE_SIZE, &hostStorePtr);
		if ((result != VK_SUCCESS) || (hostStorePtr == nullptr))
		{
			//Error?
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryBlock::InitVKDeviceMemoryBlock() - Could not map memory to obtain host store pointer.\n");
			return false;
		}
	}

	//Store is now active
	isStoreActive = true;
	
	//Done
	return true;
}

void VulkanDeviceMemoryStore::Shutdown()
{
	//Clear all blocks in this store
	std::list<EngineAPI::Graphics::DeviceMemoryBlock>::iterator it = deviceMemoryBlocksList.begin();
	while (it != deviceMemoryBlocksList.end())
	{
		it->Shutdown();
		++it;
	}
	deviceMemoryBlocksList.clear();

	//Free the memory block
	if (vkMemoryStoreHandle)
	{
		vkFreeMemory(cachedVkLogicalDevice, vkMemoryStoreHandle, nullptr);
		vkMemoryStoreHandle = VK_NULL_HANDLE;
	}

	memoryStoreSizeBytes = 0;
	isStoreActive = false;
}

EngineAPI::Graphics::DeviceMemoryBlock* VulkanDeviceMemoryStore::SearchExistingBlocksListToUseToSuballocResource(
	EngineAPI::Rendering::Resource* resource,
	VkDeviceSize blockSizeNeeded, VkDeviceSize resourceAlignment)
{
	EngineAPI::Graphics::DeviceMemoryBlock* out = nullptr;

	std::list<EngineAPI::Graphics::DeviceMemoryBlock>::iterator it = deviceMemoryBlocksList.begin();
	while (it != deviceMemoryBlocksList.end())
	{
		//TODO 1: Should really be looking for the smallest block which we can use
		//for this resource (not the first) -> reduces memory loss when
		//a big block is used with a small resource
		//
		//TODO 2: Blocks should be able to move to handle memory alignment requirements. 

		//Is it a) free, b) large enough, c) memory aligned for this resource 
		if (it->IsBlockFree())
		{
			VkDeviceSize newOffset = it->GetBlockOffsetInStoreBytes();
			VkDeviceSize alignment = (newOffset % resourceAlignment);

			//Check alignment. If not aligned already, align the memory
			bool isAligned = alignment == 0;
			if (!isAligned)
				newOffset += alignment; //Shift offset to the right

			if (newOffset % resourceAlignment != 0)
			{
				//Error
				EngineAPI::Debug::DebugLog::PrintErrorMessage("Error: Not aligned - fix this bug!\n");
			}

			//Do we have enough space when factoring memory alignment requirements
			VkDeviceSize newUsableSize = it->GetBlockSizeBytes() - alignment; //Factor in alignment requirments 
			if (newUsableSize >= blockSizeNeeded)
			{
				//Can re use this block!
				return &(*it);
			}
		}

		//Next block in list
		++it;
	}

	return out;
}


bool VulkanDeviceMemoryStore::Private_Suballoc(EngineAPI::Rendering::Resource* resource,
	VkDeviceSize blockSize, VkDeviceSize resourceAlignment)
{
	//Can we reuse an existing block that is now free?
	EngineAPI::Graphics::DeviceMemoryBlock* existingBlock =
		SearchExistingBlocksListToUseToSuballocResource(resource, blockSize, resourceAlignment);
	if (existingBlock)
	{
		//Found existing block to use. 
		if (!existingBlock->InitVKMemoryBlock((DeviceMemoryStore*)this,
			resource, existingBlock->GetBlockSizeBytes(), existingBlock->GetBlockOffsetInStoreBytes(), vkIsStoreMemoryMappable))
		{
			//Error
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryStore::Private_Suballoc() - Error when reusing block for new suballocation\n");
			return false;
		}
	}
	else
	{
		//Else, we will have to alloc a new block
		//
		//Calculate offset within the VkDeviceBlock for the beginning of this resource
		VkDeviceSize memoryBlockOffset = 0;
		if (lastSuballocedBlock)
			memoryBlockOffset = lastSuballocedBlock->GetBlockOffsetInStoreBytes() + lastSuballocedBlock->GetBlockSizeBytes();

		//Sub alloc from within this store
		EngineAPI::Graphics::DeviceMemoryBlock newBlock;

		if (!newBlock.InitVKMemoryBlock((DeviceMemoryStore*)this,
			resource, blockSize, memoryBlockOffset, vkIsStoreMemoryMappable))
		{
			//Error
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryStore::Private_Suballoc() - Error suballocing DeviceMemoryBlock\n");
			return false;
		}
		
		newBlock.isFree = true; //TEMP

		//Add the newly created block to list
		deviceMemoryBlocksList.push_back(newBlock);

		//Update last suballoced block pointer to point to the end of the 
		//list
		lastSuballocedBlock = &deviceMemoryBlocksList.back();
	}
	//Done
	return true;
}

void VulkanDeviceMemoryStore::Private_FreeBlock(EngineAPI::Graphics::DeviceMemoryBlock* block)
{
	//TODO:
	//
	//Currently just sets the block to true
	//auto blockItterator(std::find(memoryBlocksArray.begin(), memoryBlocksArray.end(), block));
}