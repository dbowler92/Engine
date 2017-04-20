//Swapchain.h
//Created 20/04/17
//Created By Daniel Bowler
//
//Double buffer && interaction with the OS windowing system

#pragma once

//Platform info
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanSwapchain.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class Swapchain : public RENDERING_PLATFORM_IMPLEMENTATION(Swapchain)
		{
		public:
			Swapchain() {};
			~Swapchain() {};
		};
	};
};