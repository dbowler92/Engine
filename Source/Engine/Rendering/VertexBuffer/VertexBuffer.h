//VertexBuffer.h
//Created 19/05/17
//Cretaed By Daniel Bowler
//
//Represents a GPU (could be CPU if mappable, mind) buffer of vertex data 

#pragma once

//Platform
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanVertexBuffer.h"
#endif

namespace EngineAPI
{
	namespace Rendering
	{
		class VertexBuffer : public RENDERING_PLATFORM_IMPLEMENTATION(VertexBuffer)
		{
		public:
			VertexBuffer() : RENDERING_PLATFORM_CLASS(VertexBuffer)(RENDERING_RESOURCE_TYPE_VERTEX_BUFFER) {};
			~VertexBuffer() {};
		};
	};
};
