#include "VulkanUniformBuffer.h"

using namespace EngineAPI::Rendering::Platform;

void VulkanUniformBuffer::Shutdown()
{
	//Shutdown vulkan uniform buffer data...

	//Shutdown super class - the buffer object
	__super::Shutdown();
}

bool VulkanUniformBuffer::InitVKUniformBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
	VkDeviceSize uniformBufferSizeBytes, bool isDynamicUniformBuffer, RenderingResourceUsage resourceUsage)
{
	//Creation info
	VkBufferCreateInfo ubCreateInfo = {};
	ubCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	ubCreateInfo.pNext = nullptr;
	ubCreateInfo.flags = 0; //TODO
	ubCreateInfo.size = uniformBufferSizeBytes;
	ubCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; //VERIFY
	ubCreateInfo.pQueueFamilyIndices = nullptr;
	ubCreateInfo.queueFamilyIndexCount = 0;
	ubCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	//Init buffer
	if (!InitVKBuffer(renderingDevice, &ubCreateInfo, resourceUsage))
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanVertexBuffer::InitVKUniformBuffer() Error - Could not init VkBuffer\n");
		return false;
	}


	//Done
	return true;
}

bool VulkanUniformBuffer::AllocAndBindVKUniformBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
	void* uniformBufferData, EngineAPI::Graphics::DeviceMemoryStore* optionalDeviceStore)
{
	assert(vkBufferHandle != VK_NULL_HANDLE);

	//Alloc resource block through the DeviceMemoryAllocator
	if (!AllocVKBufferMemoryBlock(renderingDevice, optionalDeviceStore))
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanUniformBuffer::AllocAndBindVKUniformBuffer() Error - Could not alloc memory block\n");
		return false;
	}

	//Staging -> Write data to the buffer
	if (uniformBufferData)
	{
		if (!WriteDataToVKBuffer(renderingDevice, uniformBufferData))
		{
			//Error
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanUniformBuffer::AllocAndBindVKUniformBuffer() Error - Could not write data to memory(Staging)\n");
			return false;
		}
	}

	//Bind device memory
	if (!BindVKBufferMemory(renderingDevice))
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanUniformBuffer::AllocAndBindVKUniformBuffer() Error - Could not bind memory on device\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanUniformBuffer::InitVKUniformViews(EngineAPI::Graphics::RenderDevice* renderingDevice)
{
	//TODO

	//Done
	return true;
}