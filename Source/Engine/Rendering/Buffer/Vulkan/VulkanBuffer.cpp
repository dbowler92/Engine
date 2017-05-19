#include "VulkanBuffer.h"

using namespace EngineAPI::Rendering::Platform;

bool VulkanBuffer::InitVKBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
	VkBufferCreateInfo* bufferCreateInfo, bool isDynamicBuffer)
{
	//Cache info
	this->cachedVKLogicalDevice = renderingDevice->GetVKLogicalDevice();
	this->isDynamicBufferFlag = isDynamicBuffer;
	this->bufferSizeBytes = bufferCreateInfo->size;

	//Create the buffer object
	VkResult result = vkCreateBuffer(cachedVKLogicalDevice, bufferCreateInfo, nullptr, &vkBufferHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanBuffer::InitVKBuffer() Error - Could not create VkBuffer object\n");
		return false;
	}

	//Done
	return true;
}

void VulkanBuffer::Shutdown()
{
	vkDestroyBuffer(cachedVKLogicalDevice, vkBufferHandle, nullptr);
	vkBufferHandle = VK_NULL_HANDLE;
}