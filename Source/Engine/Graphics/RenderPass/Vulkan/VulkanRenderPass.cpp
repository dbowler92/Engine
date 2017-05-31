#include "VulkanRenderPass.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanRenderPass::Shutdown()
{
	vkDestroyRenderPass(cachedVkDevice, vkRenderPassHandle, nullptr);
	vkRenderPassHandle = VK_NULL_HANDLE;
}

bool VulkanRenderPass::InitVKRenderPass(EngineAPI::Graphics::RenderDevice* renderingDevice,
	VkRenderPassCreateInfo* renderPassCreateInfo)
{
	//Cache device
	cachedVkDevice = renderingDevice->GetVKLogicalDevice();

	//Create the render pass
	VkResult result = vkCreateRenderPass(cachedVkDevice, renderPassCreateInfo, nullptr, &vkRenderPassHandle);
	if (result != VK_SUCCESS)
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderPass::InitVKRenderPass(): vkCreateRenderPass failed\n");
		return false;
	}

	//Done
	return true;
}

VkExtent2D VulkanRenderPass::CalculateRenderPassAreaGranularity()
{
	VkExtent2D out;
	vkGetRenderAreaGranularity(cachedVkDevice, vkRenderPassHandle, &out);
	return out;
}