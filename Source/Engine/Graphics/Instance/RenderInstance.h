//RenderInstance.h
//Created 20/04/17
//Created By Daniel Bowler
//
//Rendering instance - inherits from a specifc platform / API

#pragma once

//Platform info
#include "../../Config/EngineConfig.h"

#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanRenderInstance.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class RenderInstance : public RENDERING_PLATFORM_IMPLEMENTATION(RenderInstance)
		{
		public:
			RenderInstance() {};
			~RenderInstance() {};
		};
	};
};
