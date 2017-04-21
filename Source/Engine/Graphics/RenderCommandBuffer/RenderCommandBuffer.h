//RenderCommandBuffer.h
//Created 21/07/17
//Created By Daniel Bowler
//
//Represents a set of commands that will be sent to the GPU for processing. 
//
//Eg: Vulkan - Abstracts the VkCommandBuffer
//Eg: D3D11  - Abstracts the ID3D11CommandList (using a deferred context) ???
//Eg: OpenGL - TBC. 

#pragma once

//Platform info
#include "../../Config/EngineConfig.h"

#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanRenderCommandBuffer.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class RenderCommandBuffer : public RENDERING_PLATFORM_IMPLEMENTATION(RenderCommandBuffer)
		{
		public:
			RenderCommandBuffer() {};
			~RenderCommandBuffer() {};
		};
	};
};
