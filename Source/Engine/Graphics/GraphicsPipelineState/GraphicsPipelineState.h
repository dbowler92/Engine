//GraphicsPipelineState.h
//Created 27/05/17
//Created By Daniel Bowler
//
//Represents a reusable object that represents the state of the graphics pipeline

#pragma once

//Platform
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanGraphicsPipelineState.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class GraphicsPipelineState : public RENDERING_PLATFORM_IMPLEMENTATION(GraphicsPipelineState)
		{
		public:
			GraphicsPipelineState() {};
			virtual ~GraphicsPipelineState() {};

		};
	};
};
