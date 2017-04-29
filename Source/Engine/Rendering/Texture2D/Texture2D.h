//Texture2D.h
//Created 29/04/17
//Created By Daniel Bowler
//
//Represents a texture2D object usable in game

#pragma once

//Platform
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanTexture2D.h"
#endif

namespace EngineAPI
{
	namespace Rendering
	{
		class Texture2D : RENDERING_PLATFORM_IMPLEMENTATION(Texture2D)
		{
		public:
			Texture2D() {};
			~Texture2D() {};
		};
	};
};