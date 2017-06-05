//UniformBuffer.h
//Created 05/07/17
//Created By Daniel Bowler
//
//(AKA Constant buffers) - Buffer objects that can be fed in to and ready within a shader 

#pragma once

//Platform
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanUniformBuffer.h"
#endif

namespace EngineAPI
{
	namespace Rendering
	{
		class UniformBuffer : public RENDERING_PLATFORM_IMPLEMENTATION(UniformBuffer)
		{
		public:
			UniformBuffer() : RENDERING_PLATFORM_CLASS(UniformBuffer)(RENDERING_RESOURCE_TYPE_UNIFORM_BUFFER) {};
			virtual ~UniformBuffer() {};
		};
	};
};