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

bool VulkanDeviceMemoryStore::DoesStoreSupportResource(const VkMemoryRequirements& resourceMemoryRequriments)
{
	//Is a bit in memoryTypeBits for this stores memory type
	return (1 << vkMemoryTypeIndex) & resourceMemoryRequriments.memoryTypeBits;
}

bool VulkanDeviceMemoryStore::DoesStoreManageMemoryBlock(EngineAPI::Graphics::DeviceMemoryBlock* block)
{
	return ((void*)this == (void*)block->GetParentStore());
}

VkDeviceSize VulkanDeviceMemoryStore::CalculateAlignedMemoryOffset(VkDeviceSize memoryBlockOffset, VkDeviceSize resourceAlignmentRequirment)
{
	VkDeviceSize alignedOffset = memoryBlockOffset;

	//How many bytes to shift this offset to the right?
	VkDeviceSize bytesToShiftToMakeAligned = 0;
	VkDeviceSize missAlignment = (alignedOffset % resourceAlignmentRequirment);

	if (missAlignment != 0) //Could already be aligned
		bytesToShiftToMakeAligned = resourceAlignmentRequirment - missAlignment;

	//Shift to the right
	alignedOffset += bytesToShiftToMakeAligned;

	//Done
	return alignedOffset;
}

EngineAPI::Graphics::DeviceMemoryBlock* VulkanDeviceMemoryStore::SearchExistingBlocksListToUseToSuballocResource(
	EngineAPI::Rendering::Resource* resource,
	VkDeviceSize blockSizeNeeded, VkDeviceSize resourceAlignment)
{
	std::list<EngineAPI::Graphics::DeviceMemoryBlock>::iterator it = deviceMemoryBlocksList.begin();
	while (it != deviceMemoryBlocksList.end())
	{
		//TODO 1: Should really be looking for the smallest block which we can use
		//for this resource (not the first) -> reduces memory loss when
		//a big block is used with a small resource
		//
		//Is it a) free, b) large enough, c) memory aligned for this resource 
		if (it->IsBlockFree())
		{
			VkDeviceSize newOffset = it->GetBlockOffsetInStoreBytes();
			VkDeviceSize bytesToShiftToMakeAligned = resourceAlignment - (newOffset % resourceAlignment);

			//Align the offset
			newOffset += bytesToShiftToMakeAligned; //Shift offset to the right

			//Do we have enough space when factoring memory alignment requirements
			VkDeviceSize newUsableSize = it->GetBlockSizeBytes() - bytesToShiftToMakeAligned; //Factor in alignment requirements (we shift the start offset to the right thus reducing the usable size of the block 
			if (newUsableSize >= blockSizeNeeded)
			{
				//Can re use this block!
				return &(*it);
			}
		}

		//Next block in list
		++it;
	}

	//No existing block available
	return nullptr;
}

SuballocationResult VulkanDeviceMemoryStore::Private_Suballoc(EngineAPI::Rendering::Resource* resource,
	VkDeviceSize blockSize, VkDeviceSize resourceAlignment, VkDeviceSize resourceSize)
{
	//Can we reuse an existing block that is now free?
	EngineAPI::Graphics::DeviceMemoryBlock* existingBlock =
		SearchExistingBlocksListToUseToSuballocResource(resource, blockSize, resourceAlignment);
	if (existingBlock)
	{
		//Found existing block to use. 
		if (!existingBlock->InitVKMemoryBlock((DeviceMemoryStore*)this,
			resource, resourceAlignment, existingBlock->GetBlockSizeBytes(), existingBlock->GetBlockOffsetInStoreBytes(), resourceSize, vkIsStoreMemoryMappable))
		{
			//Error
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryStore::Private_Suballoc() - Error when reusing block for new suballocation\n");
			return ALLOCATION_RESULT_BLOCK_INIT_FAILED;
		}
	}
	else
	{
		//Else, we will have to alloc a new block
		//
		//Calculate offset within the VkDeviceBlock for the beginning of this resource
		VkDeviceSize memoryBlockOffset = 0;
		VkDeviceSize memoryBlockOffsetAligned = 0;
		if (lastSuballocedBlock)
		{
			//Start offset (in to the entire store) for this new block
			memoryBlockOffset = lastSuballocedBlock->GetBlockOffsetInStoreBytes() + lastSuballocedBlock->GetBlockSizeBytes();

			//Align memory offset for the new block
			memoryBlockOffsetAligned = CalculateAlignedMemoryOffset(memoryBlockOffset, resourceAlignment);
		}

		//Check if we would overrun the memory store with this new block
		VkDeviceSize endOfNewSuballoc = memoryBlockOffsetAligned + blockSize;
		if (endOfNewSuballoc > memoryStoreSizeBytes)
		{
			//Suballoc would go out of bounds - print message
			EngineAPI::Debug::DebugLog::PrintWarningMessage("VulkanDeviceMemoryStore::Private_Suballoc() - Resource allocation would exceed bounds of the store. Try a differnt store or create a new one\n");
			return ALLOCATION_RESULT_OUT_OF_MEMORY_FOR_STORE;
		}

		//Block size needs adjusting to factor in potential alignment issues. (thus, the block can fit
		//the resource and padding (shift the offset to the right) as a result of memory
		//alignment. 
		VkDeviceSize blockSizeIncreasedForAlignment = (memoryBlockOffsetAligned - memoryBlockOffset) + blockSize;

		//Suballoc a new block from within this store
		EngineAPI::Graphics::DeviceMemoryBlock newBlock;
		if (!newBlock.InitVKMemoryBlock((DeviceMemoryStore*)this,
			resource, resourceAlignment, blockSizeIncreasedForAlignment, memoryBlockOffset, resourceSize, vkIsStoreMemoryMappable))
		{
			//Error
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryStore::Private_Suballoc() - Error suballocing DeviceMemoryBlock\n");
			return ALLOCATION_RESULT_BLOCK_INIT_FAILED;
		}
		
		//Add the newly created block to list
		deviceMemoryBlocksList.push_back(newBlock);

		//Update last suballoced block pointer to point to the end of the 
		//list
		lastSuballocedBlock = &deviceMemoryBlocksList.back();
	}

	//Done
	return ALLOCATION_RESULT_SUCCESS;
}

SuballocationResult VulkanDeviceMemoryStore::Private_SuballocExistingBlock(EngineAPI::Rendering::Resource* resource,
	VkDeviceSize resourceAlignment, VkDeviceSize resourceSize,
	EngineAPI::Graphics::DeviceMemoryBlock* block)
{
	//Check
	if (!DoesStoreManageMemoryBlock(block))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryStore::Private_SuballocExistingBlock() - Store does not manage supplied block\n");
		return ALLOCATION_RESULT_STORE_DOES_NOT_MANAGE_BLOCK;
	}

	//Use passed in block 
	if (!block->InitVKMemoryBlock((DeviceMemoryStore*)this,
		resource, resourceAlignment, block->GetBlockSizeBytes(), block->GetBlockOffsetInStoreBytes(), resourceSize, vkIsStoreMemoryMappable))
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryStore::Private_SuballocExistingBlock() - Error when reusing block for new suballocation\n");
		return ALLOCATION_RESULT_BLOCK_INIT_FAILED;
	}

	//Done
	return ALLOCATION_RESULT_SUCCESS;
}

void VulkanDeviceMemoryStore::Private_FreeBlock(EngineAPI::Graphics::DeviceMemoryBlock* block)
{
	//TODO:
	//
	//Currently just sets the block to true
	//auto blockItterator(std::find(memoryBlocksArray.begin(), memoryBlocksArray.end(), block));
}