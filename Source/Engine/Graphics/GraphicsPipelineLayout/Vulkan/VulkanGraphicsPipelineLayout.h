//VulkanGraphicsPipelineLayout.h
//Created 05/06/17
//Created By Daniel Bowler
//
//Descriptor layout used alongside the pipeline state

#pragma once

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Device
#include "../../Device/RenderDevice.h"

//Vulkan header
#include <vulkan/vulkan.h>

//Knows about the descriptor sets
namespace EngineAPI
{
	namespace Graphics
	{
		class DescriptorSet;
	};
};

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanGraphicsPipelineLayout
			{
			public:
				VulkanGraphicsPipelineLayout() {};
				virtual ~VulkanGraphicsPipelineLayout() = 0 {};

				//Shutdown the descriptor object
				void Shutdown();

				//Inits the pipeline layout object
				bool InitVKGraphicsPipelineLayout(EngineAPI::Graphics::RenderDevice* renderingDevice, 
					EngineAPI::Graphics::DescriptorSet* descriptorSetsArray, uint32_t descriptorSetsCount);

			public:
				//Gets the pipeline layout handle.
				VkPipelineLayout GetVKPipelineLayoutHandle() { return vkPipelineLayoutHandle; };

			protected:
				//Pipeline layout handle
				VkPipelineLayout vkPipelineLayoutHandle = VK_NULL_HANDLE;

				//Logical device used to create this pipeline layout
				VkDevice cachedVKLogicalDevice = VK_NULL_HANDLE;
			};
		};
	};
};
