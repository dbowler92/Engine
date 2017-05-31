//DrawableObject.h
//Created 31/05/17
//Created By Daniel Bowler
//
//Test class (for now) which will be used to hold data required for
//objects that need to be rendered to the screen. 

#pragma once

//Platform
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanDrawableObject.h"
#endif

namespace EngineAPI
{
	namespace Rendering
	{
		class DrawableObject : public RENDERING_PLATFORM_IMPLEMENTATION(DrawableObject)
		{
		public:
			DrawableObject() {};
			virtual ~DrawableObject() {};

		};
	};
};