//RenderPassInstance.h
//Created 01/06/17
//Created By Daniel Bowler
//
//Represents a single render pass instance. Will have its 
//command buffer filled with drawing commands. 

#pragma once

//Platform info
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanRenderPassInstance.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class RenderPassInstance : public RENDERING_PLATFORM_IMPLEMENTATION(RenderPassInstance)
		{
		public:
			RenderPassInstance() {};
			~RenderPassInstance() {};
		};
	};
};