#include "VulkanDescriptorPool.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanDescriptorPool::Shutdown()
{
	if (vkDescriptorPoolHandle)
	{
		vkDestroyDescriptorPool(cachedVKLogicalDevice, vkDescriptorPoolHandle, nullptr);
		vkDescriptorPoolHandle = VK_NULL_HANDLE;
	}
}

bool VulkanDescriptorPool::InitVKDescriptorPool(EngineAPI::Graphics::RenderDevice* renderingDevice)
{
	//Cache device
	cachedVKLogicalDevice = renderingDevice->GetVKLogicalDevice();

	//Description
	VkDescriptorPoolCreateInfo poolCreateInfo = {};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.pNext = nullptr;
	poolCreateInfo.flags = 0; //TODO
	//poolCreateInfo.

	//Create pool
	VkResult result = vkCreateDescriptorPool(cachedVKLogicalDevice, &poolCreateInfo, nullptr, &vkDescriptorPoolHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDescriptorPool::InitVKDescriptorPool() Error: Could not create descriptor pool\n");
		return false;
	}

	//Done
	return true;
}