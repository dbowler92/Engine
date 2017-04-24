//CommandBufferPool
//Created 24/04/17
//Created By Daniel Bowler
//
//Pool used to allocate / get a RenderCommandBuffer

#pragma once

//Platform info
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanCommandBufferPool.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class CommandBufferPool : public RENDERING_PLATFORM_IMPLEMENTATION(CommandBufferPool)
		{
		public:
			CommandBufferPool() {};
			~CommandBufferPool() {};
		};
	};
};