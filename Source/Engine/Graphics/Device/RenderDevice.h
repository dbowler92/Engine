//RenderDevice.h
//Created 20/04/17
//Created By Daniel Bowler
//
//Represents the physical and logical device for rendering.

#pragma once

//Platform info
#include "../../Config/EngineConfig.h"

#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanRenderDevice.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class RenderDevice : public RENDERING_PLATFORM_IMPLEMENTATION(RenderDevice)
		{
		public:
			RenderDevice() {};
			~RenderDevice() {};


		};
	};
};