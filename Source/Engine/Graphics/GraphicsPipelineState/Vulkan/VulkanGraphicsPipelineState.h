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

			};
		};
	};
};
