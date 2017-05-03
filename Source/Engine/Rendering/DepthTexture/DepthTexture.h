//DepthTexture.h
//Created 03/05/17
//Created By Daniel Bowler
//
//Depth buffer to render depth info in to (eg: in conjunction with the swapchain colour
//buffers and shadow mapping). 

#pragma once

//Platform
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanDepthTexture.h"
#endif

namespace EngineAPI
{
	namespace Rendering
	{
		class DepthTexture : public RENDERING_PLATFORM_IMPLEMENTATION(DepthTexture)
		{
		public:
			DepthTexture() {};
			~DepthTexture() {};
		};
	};
};
