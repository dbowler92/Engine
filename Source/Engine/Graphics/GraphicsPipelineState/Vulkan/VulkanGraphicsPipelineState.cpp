#include "VulkanGraphicsPipelineState.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanGraphicsPipelineState::Shutdown()
{
	if (vkPipelineLayout)
	{
		vkDestroyPipelineLayout(cachedVKLogicalDevice, vkPipelineLayout, nullptr);
		vkPipelineLayout = VK_NULL_HANDLE;
	}

	if (vkPipelineHandle)
	{
		vkDestroyPipeline(cachedVKLogicalDevice, vkPipelineHandle, nullptr);
		vkPipelineHandle = VK_NULL_HANDLE;
	}
}

bool VulkanGraphicsPipelineState::InitVKGraphicsPipelineState(EngineAPI::Graphics::RenderDevice* renderingDevice, 
	EngineAPI::Graphics::GraphicsPipelineCache* optionalPipelineCache, EngineAPI::Graphics::RenderPass* renderPass,
	EngineAPI::Graphics::Program* program, PipelineStateDescription* pipelineState, bool createUsingOptimiseFlag)
{
	//Cache data
	cachedVKLogicalDevice = renderingDevice->GetVKLogicalDevice();
	
	//Pipeline layout
	VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
	pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	VkResult result = vkCreatePipelineLayout(cachedVKLogicalDevice, &pPipelineLayoutCreateInfo, NULL, &vkPipelineLayout);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsPipelineState::InitVKGraphicsPipelineState(): Error creating pipeline layout\n");
		return false;
	}

	//Description
	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
	graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineCreateInfo.pNext = nullptr;
	graphicsPipelineCreateInfo.flags = 0; //TODO
	if (createUsingOptimiseFlag)
		graphicsPipelineCreateInfo.flags = VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
	
	//States
	graphicsPipelineCreateInfo.pVertexInputState = pipelineState->vertexInputStateInfo;
	graphicsPipelineCreateInfo.pInputAssemblyState = pipelineState->inputAssemblyInfo;
	graphicsPipelineCreateInfo.pRasterizationState = pipelineState->rasterStateInfo;
	graphicsPipelineCreateInfo.pColorBlendState = pipelineState->colourBlendStateInfo;
	graphicsPipelineCreateInfo.pTessellationState = pipelineState->tessStateInfo;
	graphicsPipelineCreateInfo.pMultisampleState = pipelineState->multiSampleStateInfo;
	graphicsPipelineCreateInfo.pDynamicState = pipelineState->dynamicState;
	graphicsPipelineCreateInfo.pViewportState = pipelineState->viewportStateInfo;
	graphicsPipelineCreateInfo.pDepthStencilState = pipelineState->depthStencilStateInfo;

	//Stages
	std::vector<VkPipelineShaderStageCreateInfo> shaderStagesArray(program->GetAttachedVKShadersCount());
	for (int i = 0; i < program->GetAttachedVKShadersCount(); i++)
		program->GetAttachedVKShaders()[i].FillVKPipelineShaderStageDescriptionStruct(&shaderStagesArray[i]);

	graphicsPipelineCreateInfo.pStages = shaderStagesArray.data();
	graphicsPipelineCreateInfo.stageCount = program->GetAttachedVKShadersCount();

	//Render pass & subpass
	graphicsPipelineCreateInfo.renderPass = renderPass->GetVKRenderPassHandle();
	graphicsPipelineCreateInfo.subpass = 0;

	//Set pipeline layout
	graphicsPipelineCreateInfo.layout = vkPipelineLayout;

	//PCO (Optional - can be null)
	VkPipelineCache optionalPCOHandle = VK_NULL_HANDLE;
	if (optionalPipelineCache)
		optionalPCOHandle = optionalPipelineCache->GetVKPipelineCacheHandle();

	//Create pipeline object
	result = vkCreateGraphicsPipelines(cachedVKLogicalDevice, optionalPCOHandle, 1, &graphicsPipelineCreateInfo, nullptr, &vkPipelineHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsPipelineState::InitVKGraphicsPipelineState() Error creating pipeline object\n");
		return false;
	}

	//Done
	return true;
}