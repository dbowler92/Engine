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
	for (int i = 0; i < deviceMemoryBlocksCount; i++)
		deviceMemoryBlocksArray[i].Shutdown();

	//Free the memory block
	if (vkMemoryStoreHandle)
	{
		vkFreeMemory(cachedVkLogicalDevice, vkMemoryStoreHandle, nullptr);
		vkMemoryStoreHandle = VK_NULL_HANDLE;
	}

	memoryStoreSizeBytes = 0;
	isStoreActive = false;
}

bool VulkanDeviceMemoryStore::Private_Suballoc(EngineAPI::Rendering::Resource* resource,
	VkDeviceSize blockSize, VkDeviceSize resourceAlignment)
{
	//Ensure that we can suballoc a block out of this store
	if (deviceMemoryBlocksCount >= ENGINE_CONFIG_VULKAN_API_MAX_NUMBER_OF_MEMORY_BLOCKS_PER_STORE)
	{
		EngineAPI::Debug::DebugLog::PrintWarningMessage("VulkanDeviceMemoryStore::Private_Suballoc() - Warning: Trying to suballoc out of a store which has reached max block count. Try another store\n");
		return false;
	}

	//Sub alloc from within this store
	VkDeviceSize memoryBlockOffset = 0; //TODO
	if (!deviceMemoryBlocksArray[deviceMemoryBlocksCount].InitVKMemoryBlock((DeviceMemoryStore*)this,
		resource, blockSize, memoryBlockOffset, vkIsStoreMemoryMappable))
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryStore::Private_Suballoc() - Error suballocing DeviceMemoryBlock\n");
		return false;
	}

	//Another block alloced
	deviceMemoryBlocksCount++;

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