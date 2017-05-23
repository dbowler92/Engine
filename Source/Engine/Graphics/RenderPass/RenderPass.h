//RenderPass.h
//Created 23/05/17
//Created By Daniel Bowler
//
//Render pass object

#pragma once

//Platform info
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanRenderPass.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class RenderPass : public RENDERING_PLATFORM_IMPLEMENTATION(RenderPass)
		{
		public:
			RenderPass() {};
			~RenderPass() {};
		};
	};
};