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

bool VulkanDescriptorPool::InitVKDescriptorPools(EngineAPI::Graphics::RenderDevice* renderingDevice,
	VkDescriptorPoolSize* pools, uint32_t poolsCount, bool doInitWithCreateFreeDescriptorSetBitFlag)
{
	//Cache device
	cachedVKLogicalDevice = renderingDevice->GetVKLogicalDevice();

	//Description
	VkDescriptorPoolCreateInfo poolCreateInfo = {};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.pNext = nullptr;

	if (doInitWithCreateFreeDescriptorSetBitFlag)
		poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	else
		poolCreateInfo.flags = 0;

	poolCreateInfo.pPoolSizes = pools;
	poolCreateInfo.poolSizeCount = poolsCount;
	poolCreateInfo.maxSets = 1; //VERIFY

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