#include "VulkanGraphicsPipelineLayout.h"

//Complete forward declarations
#include "../../DescriptorSet/DescriptorSet.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanGraphicsPipelineLayout::Shutdown()
{
	if (vkPipelineLayoutHandle)
	{
		vkDestroyPipelineLayout(cachedVKLogicalDevice, vkPipelineLayoutHandle, nullptr);
		vkPipelineLayoutHandle = VK_NULL_HANDLE;
	}
}

bool VulkanGraphicsPipelineLayout::InitVKGraphicsPipelineLayout(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Graphics::DescriptorSet* descriptorSetsArray, uint32_t descriptorSetsCount)
{
	//Cache device
	cachedVKLogicalDevice = renderingDevice->GetVKLogicalDevice();

	//Description
	VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
	pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pPipelineLayoutCreateInfo.pNext = nullptr;
	pPipelineLayoutCreateInfo.flags = 0; //Verify
	
	//Descriptor sets 
	std::vector<VkDescriptorSetLayout> descriptorSetsLayoutVK(descriptorSetsCount);
	for (int i = 0; i < descriptorSetsCount; ++i)
		descriptorSetsLayoutVK[i] = descriptorSetsArray[i].GetVKDescriptorSetLayoutHandle();

	pPipelineLayoutCreateInfo.pSetLayouts = descriptorSetsCount > 0 ? descriptorSetsLayoutVK.data() : nullptr;
	pPipelineLayoutCreateInfo.setLayoutCount = descriptorSetsCount;

	//Push constants
	pPipelineLayoutCreateInfo.pPushConstantRanges = nullptr; //TODO
	pPipelineLayoutCreateInfo.pushConstantRangeCount = 0; //TODO

	VkResult result = vkCreatePipelineLayout(cachedVKLogicalDevice, &pPipelineLayoutCreateInfo, NULL, &vkPipelineLayoutHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsPipelieLayout::InitVKGraphicsPipelineLayout(): Error creating pipeline layout\n");
		return false;
	}

	//Done
	return true;
}