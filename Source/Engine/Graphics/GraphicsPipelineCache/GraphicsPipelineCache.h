//GraphicsPipelineCache.h
//Created 27/05/17
//Created By Daniel Bowler
//
//Represents a PCO (Grpahics)

#pragma once

//Platform
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanGraphicsPipelineCache.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class GraphicsPipelineCache : public RENDERING_PLATFORM_IMPLEMENTATION(GraphicsPipelineCache)
		{
		public:
			GraphicsPipelineCache() {};
			virtual ~GraphicsPipelineCache() {};

		};
	};
};

