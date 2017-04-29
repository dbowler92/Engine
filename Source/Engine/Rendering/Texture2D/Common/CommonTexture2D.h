//CommonTexture2D.h
//Created 29/04/17
//Created By Daniel Bowler
//
//Interface that each platform specific implementation of texture2D objects
//will inherit from
//
//Note: This class inherits from TextureResource -> Which will contain the code
//for generic texture types (eg: Vulkan -> VkImage)

#pragma once

//Base
#include "../../TextureResource/TextureResource.h"

namespace EngineAPI
{
	namespace Rendering
	{
		namespace Interface
		{
			class CommonTexture2D : public EngineAPI::Rendering::TextureResource
			{
			public:
				CommonTexture2D() {};
				virtual ~CommonTexture2D() = 0 {};
			};
		};
	};
};