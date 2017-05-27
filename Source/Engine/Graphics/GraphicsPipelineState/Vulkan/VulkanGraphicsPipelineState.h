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

//Vulkan header
#include <vulkan/vulkan.h>

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
				bool InitVKGraphicsPipelineState(EngineAPI::Graphics::RenderDevice* renderingDevice);

			protected:
				//Pipeline handle
				VkPipeline vkPipelineHandle = VK_NULL_HANDLE;

				//Cached device used to create this pipeline object
				VkDevice cachedVKLogicalDevice = VK_NULL_HANDLE;
			};
		};
	};
};
