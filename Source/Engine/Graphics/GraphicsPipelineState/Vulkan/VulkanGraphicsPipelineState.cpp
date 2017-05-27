#include "VulkanGraphicsPipelineState.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanGraphicsPipelineState::Shutdown()
{
	if (vkPipelineHandle)
	{
		vkDestroyPipeline(cachedVKLogicalDevice, vkPipelineHandle, nullptr);
		vkPipelineHandle = VK_NULL_HANDLE;
	}
}

bool VulkanGraphicsPipelineState::InitVKGraphicsPipelineState(EngineAPI::Graphics::RenderDevice* renderingDevice)
{
	//Cache data
	cachedVKLogicalDevice = renderingDevice->GetVKLogicalDevice();

	//Description
	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
	graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineCreateInfo.pNext = nullptr;
	graphicsPipelineCreateInfo.flags = 0; //TODO

	//Other data required at init time.
	VkPipelineCache pipelineCache = VK_NULL_HANDLE; //Can be null

	//Create pipeline object
	VkResult result = vkCreateGraphicsPipelines(cachedVKLogicalDevice, pipelineCache, 1, &graphicsPipelineCreateInfo, nullptr, &vkPipelineHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsPipelineState::InitVKGraphicsPipelineState() Error creating pipeline object\n");
		return false;
	}

	//Done
	return true;
}