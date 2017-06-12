#include "VulkanStagingBuffer.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanStagingBuffer::Shutdown()
{
	//Cleanup staging buffer data

	//Shutdown super class - the buffer object
	__super::Shutdown();
}

bool VulkanStagingBuffer::InitVKStagingBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
	VkDeviceSize stagingBufferSizeBytes)
{
	VkBufferCreateInfo stagingBufferCreateInfo = {};
	stagingBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	stagingBufferCreateInfo.pNext = nullptr;
	stagingBufferCreateInfo.flags = 0; //TODO
	stagingBufferCreateInfo.size = stagingBufferSizeBytes;
	stagingBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	
	//TODO: If we want to use a queue family to transfer data that is *not* the main
	//graphics/compute family, we need to modify the sharing mode. 
	stagingBufferCreateInfo.pQueueFamilyIndices = nullptr;
	stagingBufferCreateInfo.queueFamilyIndexCount = 0;
	stagingBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	//Init the buffer
	if (!InitVKBuffer(renderingDevice, &stagingBufferCreateInfo, RENDERING_RESOURCE_USAGE_GPU_READ_CPU_READ_WRITE))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanStagingBuffer::InitVKStagingBuffer() Error - Could not init VkBuffer\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanStagingBuffer::AllocAndBindHostVisibleVKStagingBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
	void* stagingData, EngineAPI::Graphics::DeviceMemoryStore* optionalDeviceStore)
{
	assert(vkBufferHandle != VK_NULL_HANDLE);
	assert(stagingData != nullptr);

	//Alloc resource block through the DeviceMemoryAllocator
	if (!AllocVKBufferMemoryBlock(renderingDevice, optionalDeviceStore))
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanStagingBuffer::AllocAndBindHostVisibleVKStagingBuffer() Error - Could not alloc memory block\n");
		return false;
	}

	//Write data to the block...
	

	//Bind device memory
	if (!BindVKBufferMemory(renderingDevice))
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanStagingBuffer::AllocAndBindHostVisibleVKStagingBuffer() Error - Could not bind memory on device\n");
		return false;
	}

	//Done
	return true;
}