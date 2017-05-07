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
#include "Vulkan\VulkanTexture.h"
#endif

namespace EngineAPI
{
	namespace Rendering
	{
		class Texture : public RENDERING_PLATFORM_IMPLEMENTATION(Texture)
		{
		public:
			Texture() {};
			virtual ~Texture() = 0 {};

			//Each resource will have a shutdown function to implement
			virtual void Shutdown() { __super::Shutdown(); };
		};
	};
};