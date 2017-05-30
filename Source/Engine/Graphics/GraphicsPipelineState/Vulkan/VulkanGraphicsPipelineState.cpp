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

bool VulkanGraphicsPipelineState::InitVKGraphicsPipelineState(EngineAPI::Graphics::RenderDevice* renderingDevice, 
	EngineAPI::Graphics::GraphicsPipelineCache* optionalPipelineCache, EngineAPI::Graphics::RenderPass* renderPass,
	EngineAPI::Graphics::Program* program, PipelineStateDescription* pipelineState)
{
	//Cache data
	cachedVKLogicalDevice = renderingDevice->GetVKLogicalDevice();
	
	//Description
	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
	graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineCreateInfo.pNext = nullptr;
	graphicsPipelineCreateInfo.flags = 0; //TODO
	
	//States
	graphicsPipelineCreateInfo.pVertexInputState = pipelineState->vertexInputStateInfo;
	graphicsPipelineCreateInfo.pInputAssemblyState = pipelineState->inputAssemblyInfo;
	graphicsPipelineCreateInfo.pRasterizationState = pipelineState->rasterStateInfo;
	graphicsPipelineCreateInfo.pColorBlendState = pipelineState->colorBlendStateInfo;
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

	//PCO (Optional - can be null)
	VkPipelineCache optionalPCOHandle = VK_NULL_HANDLE;
	if (optionalPipelineCache)
		optionalPCOHandle = optionalPipelineCache->GetVKPipelineCacheHandle();

	//Create pipeline object
	VkResult result = vkCreateGraphicsPipelines(cachedVKLogicalDevice, optionalPCOHandle, 1, &graphicsPipelineCreateInfo, nullptr, &vkPipelineHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsPipelineState::InitVKGraphicsPipelineState() Error creating pipeline object\n");
		return false;
	}

	//Done
	return true;
}