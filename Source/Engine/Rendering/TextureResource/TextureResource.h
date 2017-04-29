//TextureResource.h
//Created 29/04/17
//Created By Daniel Bowler
//
//Represents a basic texture object - When using textures in game, you will
//not use this class -> Rather, see things like Texture2D, Texture2DArray etc etc etc
//
//As a result, this will form the base of those classes (CommonTexture2D will inherit
//from TextureResource -> Which will inherit from a platform specific implementation of
//basic texture objects)

#pragma once

//Platform info
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanTextureResource.h"
#endif

namespace EngineAPI
{
	namespace Rendering
	{
		class TextureResource : public RENDERING_PLATFORM_IMPLEMENTATION(TextureResource)
		{
		public:
			TextureResource() {};
			virtual ~TextureResource() = 0 {}; //Abstract class - TextureResource should not be created manually
		};
	};
};
