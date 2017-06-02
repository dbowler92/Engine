//IndexBuffer.h
//Created 02/06/17
//Created By Daniel Bowler
//
//Represents an index buffer.

#pragma once


//Platform
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanIndexBuffer.h"
#endif

namespace EngineAPI
{
	namespace Rendering
	{
		class IndexBuffer : public RENDERING_PLATFORM_IMPLEMENTATION(IndexBuffer)
		{
		public:
			IndexBuffer() : RENDERING_PLATFORM_CLASS(IndexBuffer)(RENDERING_RESOURCE_TYPE_INDEX_BUFFER) {};
			~IndexBuffer() {};
		};
	};
};
