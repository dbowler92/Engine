#include "VulkanCommandBufferPool.h"

using namespace EngineAPI::Graphics::Platform;

bool VulkanCommandBufferPool::InitVKCommandBufferPool(VkDevice* vulkanLogicalDevice,
	VkCommandPoolCreateInfo* poolCreateInfo)
{
	//Cache logical device
	cachedVkLogicalDeviceHandle = *vulkanLogicalDevice;

	//Cache state
	cachedVkQueueFamilyIndexForSubmission = poolCreateInfo->queueFamilyIndex;
	cachedVkCommandPoolFlags = poolCreateInfo->flags;

	//Create pool....
	VkResult result = vkCreateCommandPool(cachedVkLogicalDeviceHandle,
		poolCreateInfo, nullptr, &vkCommandPoolHandle);
	if (result != VK_SUCCESS)
	{
		//Error:
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommandBufferPool: Error when creating Vulkan command pool\n");
		return false;
	}

	//Done
	return true;
}

void VulkanCommandBufferPool::Shutdown()
{
	vkDestroyCommandPool(cachedVkLogicalDeviceHandle, vkCommandPoolHandle, nullptr);
}