//GraphicsPipelineLayout.h
//Created 05/06/17
//Created By Daniel Bowler
//
//Describes the pipeline layout -> Works alongside the state
//and represents inputs to the pipeline (eg: CBuffers, samplers etc)

#pragma once

//Platform
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanGraphicsPipelineLayout.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class GraphicsPipelineLayout : public RENDERING_PLATFORM_IMPLEMENTATION(GraphicsPipelineLayout)
		{
		public:
			GraphicsPipelineLayout() {};
			virtual ~GraphicsPipelineLayout() {};

		};
	};
};

