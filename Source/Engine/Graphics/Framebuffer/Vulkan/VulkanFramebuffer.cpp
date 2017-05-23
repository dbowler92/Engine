#include "VulkanFramebuffer.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanFramebuffer::Shutdown()
{
	vkDestroyFramebuffer(cachedVkDevice, vkFramebufferHandle, nullptr);
	vkFramebufferHandle = VK_NULL_HANDLE;
}

bool VulkanFramebuffer::InitVKFramebuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
	VkFramebufferCreateInfo* framebufferCreateInfo)
{
	//Cache device
	cachedVkDevice = renderingDevice->GetVKLogicalDevice();

	//Create the render pass
	VkResult result = vkCreateFramebuffer(cachedVkDevice, framebufferCreateInfo, nullptr, &vkFramebufferHandle);
	if (result != VK_SUCCESS)
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanFramebuffer::InitVKFramebuffer(): vkCreateFramebuffer failed\n");
		return false;
	}

	//Done
	return true;
}