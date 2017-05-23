//Framebuffer.h
//Created 23/05/17
//Created By Daniel Bowler
//
//Framebuffer object

#pragma once

//Platform info
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanFramebuffer.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class Framebuffer : public RENDERING_PLATFORM_IMPLEMENTATION(Framebuffer)
		{
		public:
			Framebuffer() {};
			~Framebuffer() {};
		};
	};
};