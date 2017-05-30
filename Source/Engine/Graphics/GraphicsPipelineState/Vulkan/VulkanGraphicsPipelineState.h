//VulkanGraphicsPipelineState.h
//Created 27/05/17
//Created By Daniel Bowler
//
//A reusable object that stores the state of the pipeline (Graphics. Compute will be handled differently though that is TBC)

#pragma once

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Device used to create this...
#include "../../../Graphics/Device/RenderDevice.h"

//Classes required when creating this pipeline state object
#include "../../../Graphics/RenderPass/RenderPass.h"
#include "../../../Graphics/GraphicsPipelineCache/GraphicsPipelineCache.h"
#include "../../../Graphics/Program/Program.h"

//Vulkan header
#include <vulkan/vulkan.h>

//Represents the pipeline state. Currently, this uses VK structs. Later, we will
//want to abstract this using custom classes
struct PipelineStateDescription
{
	VkPipelineDynamicStateCreateInfo* dynamicState;
	VkPipelineVertexInputStateCreateInfo* vertexInputStateInfo;
	VkPipelineInputAssemblyStateCreateInfo* inputAssemblyInfo;
	VkPipelineRasterizationStateCreateInfo* rasterStateInfo;

	VkPipelineColorBlendAttachmentState* colorBlendAttachmentStateInfo;
	uint32_t colourBlendAttachmentStateCount; //One per render target

	VkPipelineColorBlendStateCreateInfo* colorBlendStateInfo;
	VkPipelineViewportStateCreateInfo* viewportStateInfo;
	VkPipelineDepthStencilStateCreateInfo* depthStencilStateInfo;
	VkPipelineMultisampleStateCreateInfo* multiSampleStateInfo;
	VkPipelineTessellationStateCreateInfo* tessStateInfo;
};

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanGraphicsPipelineState
			{
			public:
				VulkanGraphicsPipelineState() {};
				virtual ~VulkanGraphicsPipelineState() = 0 {};

				//Shutsdown the pipeline object
				void Shutdown();

				//Inits the VkPipeline object
				bool InitVKGraphicsPipelineState(EngineAPI::Graphics::RenderDevice* renderingDevice, 
					EngineAPI::Graphics::GraphicsPipelineCache* optionalPipelineCache, EngineAPI::Graphics::RenderPass* renderPass,
					EngineAPI::Graphics::Program* program, PipelineStateDescription* pipelineState);

			protected:
				//Pipeline handle
				VkPipeline vkPipelineHandle = VK_NULL_HANDLE;

				//Cached device used to create this pipeline object
				VkDevice cachedVKLogicalDevice = VK_NULL_HANDLE;
			};
		};
	};
};
