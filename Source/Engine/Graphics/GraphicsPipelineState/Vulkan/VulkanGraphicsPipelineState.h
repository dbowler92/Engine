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
#include "../../GraphicsPipelineLayout/GraphicsPipelineLayout.h"
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
	VkPipelineColorBlendStateCreateInfo* colourBlendStateInfo;
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
				//
				//NOTE: You must pass a valid pipeline layout even if you do not use descriptor sets -> 
				//In this case, when creating a pipeline layout, just pass null and 0 as params and it
				//will create a blank/default pipeline layout object. 
				bool InitVKGraphicsPipelineState(EngineAPI::Graphics::RenderDevice* renderingDevice, 
					EngineAPI::Graphics::GraphicsPipelineCache* optionalPipelineCache, EngineAPI::Graphics::RenderPass* renderPass,
					EngineAPI::Graphics::Program* program, PipelineStateDescription* pipelineState, 
					EngineAPI::Graphics::GraphicsPipelineLayout* pipelineLayout,
					bool createUsingOptimiseFlag);

			public:
				//Returns the Vulkan pipeline object
				VkPipeline GetVKPipelineHandle() { return vkPipelineHandle; };

			protected:
				//Pipeline handle
				VkPipeline vkPipelineHandle = VK_NULL_HANDLE;

				//Pipeline layout -> Copies the handle from passed in pipelineLayout object at init
				//time. This class does *not* clean this up as it doesnt create/manage it - another
				//object will be responsible for creating it and thus, deleting it!
				VkPipelineLayout vkPipelineLayoutHandleCopy = VK_NULL_HANDLE;

				//Cached device used to create this pipeline object
				VkDevice cachedVKLogicalDevice = VK_NULL_HANDLE;
			};
		};
	};
};
