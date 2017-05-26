//Shader.h
//Created 26/05/17
//Created by Daniel Bowler
//
//Represents a single shader (vertex shader, fragment shader etc). Several of these (or just the compute shader)
//are connected to a program object and/or material object 

#pragma once

//Platform
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanShader.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class Shader : public RENDERING_PLATFORM_IMPLEMENTATION(Shader)
		{
		public:
			Shader() {};
			virtual ~Shader(){};

		};
	};
};
