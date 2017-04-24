//GraphicsCommandQueueFamily.h
//Created 24/04/17
//Created By Daniel Bowler
//
//Represents a queue family that contains queues which render commands (buffers) will use to be
//sent to the GPU

#pragma once

//Platform info
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanGraphicsCommandQueueFamily.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class GraphicsCommandQueueFamily : public RENDERING_PLATFORM_IMPLEMENTATION(GraphicsCommandQueueFamily)
		{
		public:
			GraphicsCommandQueueFamily() {};
			~GraphicsCommandQueueFamily() {};
		};
	};
};