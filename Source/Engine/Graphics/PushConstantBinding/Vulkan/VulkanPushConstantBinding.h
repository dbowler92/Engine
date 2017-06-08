//VulkanPushConstantBinding.h
//Created 08/06/17
//Created By Daniel Bowler
//
//Vulkan provides an optimized path to update shader constants
//via push constants (see: VulkanGraphicsPipelineLayout)

#pragma once

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Device
#include "../../../Graphics/Device/RenderDevice.h"

//Vulkan header
#include <vulkan/vulkan.h>

namespace EngineAPI
{
	namespace Graphics
	{
		class GraphicsPipelineLayout;
	};
};

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanPushConstantBinding
			{
			public:
				VulkanPushConstantBinding() {};
				virtual ~VulkanPushConstantBinding() = 0 {};

				//Shutdown the push constant binding data
				void Shutdown();

				//Inits the push constant binding
				bool InitVKPushConstantBinding(EngineAPI::Graphics::RenderDevice* renderingDevice, 
					VkShaderStageFlags shaderStagesFlag, uint32_t offset, uint32_t sizeBytes);

				//Updates the contents of the push constant buffer. 
				bool UpdateVKPushConstantBindingData(EngineAPI::Graphics::CommandQueue* commandQueue,
					EngineAPI::Graphics::GraphicsPipelineLayout* pipelineLayout, void* data);

			public:
				//Returns the VK struct representing this block of push constant data
				VkPushConstantRange& GetVKPushConstantBindingRangeData() { return vkPushConstantBindingRange; };

			protected:
				//Vulkan struct which represents the push constant binding data.
				VkPushConstantRange vkPushConstantBindingRange;

				//Command buffer used to update the push constants
				VkCommandBuffer vkPushConstantUpdateCmdBuffer = VK_NULL_HANDLE;
				
			};
		};
	};
};

