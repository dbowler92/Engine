//Program.h
//Created 30/05/17
//Created By Daniel Bowler
//
//Like a GlProgram -> Links a list of shaders together. 

#pragma once

//Platform
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanProgram.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class Program : public RENDERING_PLATFORM_IMPLEMENTATION(Program)
		{
		public:
			Program() {};
			virtual ~Program() {};

		};
	};
};