#include "VulkanDescriptorSet.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanDescriptorSet::Shutdown()
{
	if (vkDescriptorSetLayoutHandle)
	{
		vkDestroyDescriptorSetLayout(cachedVKLogicalDevice, vkDescriptorSetLayoutHandle, nullptr);
		vkDescriptorSetLayoutHandle = VK_NULL_HANDLE;
	}
}

bool VulkanDescriptorSet::InitVKDescriptorSet(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Graphics::DescriptorBinding* descriptorBindingsArray, uint32_t bindingsCount)
{
	//Cache device
	cachedVKLogicalDevice = renderingDevice->GetVKLogicalDevice();

	//Create array of VkDescriptorSetLayoutBindings -> One for each input binding
	std::vector<VkDescriptorSetLayoutBinding> layoutBidings(bindingsCount);
	for (int i = 0; i < bindingsCount; ++i)
		layoutBidings[i] = descriptorBindingsArray[i].GetVKDescriptorSetLayoutBindingDescription();

	//Create layout
	VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutCreateInfo.pNext = nullptr;
	layoutCreateInfo.bindingCount = 0;
	layoutCreateInfo.pBindings = bindingsCount > 0 ? layoutBidings.data() : nullptr;
	layoutCreateInfo.bindingCount = bindingsCount;

	VkResult result = vkCreateDescriptorSetLayout(cachedVKLogicalDevice, &layoutCreateInfo, nullptr, &vkDescriptorSetLayoutHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDescriptorSet::InitVKDescriptorSet() Error: Could not create desriptor set layout\n");
		return false;
	}

	//Done
	return true;
}