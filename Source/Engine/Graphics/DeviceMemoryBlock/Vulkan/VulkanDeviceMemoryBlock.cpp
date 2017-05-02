#include "VulkanDeviceMemoryBlock.h"

using namespace EngineAPI::Graphics::Platform;

bool VulkanDeviceMemoryBlock::InitVKDeviceMemoryBlock(VkDevice* logicalDevice, 
	VkDeviceSize deviceMemorySizeInBytesToAlloc,
	VkPhysicalDeviceMemoryProperties* fullDeviceMemoryProperties,
	uint32_t memoryTypeIndex)
{
	//Cache the logical device
	cachedVkLogicalDevice = *logicalDevice;

	//Are we requesting more bytes than is available?
	uint32_t heapIndex = fullDeviceMemoryProperties->memoryTypes[memoryTypeIndex].heapIndex;
	VkDeviceSize maxHeapSize = fullDeviceMemoryProperties->memoryHeaps[heapIndex].size;
	if (deviceMemorySizeInBytesToAlloc > maxHeapSize)
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryBlock::InitVKDeviceMemoryBlock() - Requested memory block size is too large for this device!\n");
		return false;
	}

	//TODO: Does this heap && type support what we need from it?


	//Alloc info struct
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.allocationSize = deviceMemorySizeInBytesToAlloc;
	allocInfo.memoryTypeIndex = memoryTypeIndex;

	//Alloc block of memory
	VkResult result = vkAllocateMemory(cachedVkLogicalDevice, &allocInfo, nullptr, &vkMemoryBlockHandle);
	if (result != VK_SUCCESS)
	{
		//Error:
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryBlock::InitVKDeviceMemoryBlock() - Error allocating device memory block.\n");
		return false;
	}

	//Cache size of the block
	memoryBlockSizeBytes = allocInfo.allocationSize;

	//Done
	return true;
}

void VulkanDeviceMemoryBlock::Shutdown()
{
	//Free the memory block
	vkFreeMemory(cachedVkLogicalDevice, vkMemoryBlockHandle, nullptr);
	vkMemoryBlockHandle = VK_NULL_HANDLE;

	memoryBlockSizeBytes = 0;
	suballocMemoryOffset = 0;
}