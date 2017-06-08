//Sampler2D.h
//Created 08/06/17
//Created By Daniel Bowler
//
//Represents a texture that can be sampled in a shader. Eg: Diffuse texture, normal map etc.

#pragma once

//Platform
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanSampler2D.h"
#endif

namespace EngineAPI
{
	namespace Rendering
	{
		class Sampler2D : public RENDERING_PLATFORM_IMPLEMENTATION(Sampler2D)
		{
		public:
			Sampler2D() : RENDERING_PLATFORM_CLASS(Sampler2D)(RENDERING_RESOURCE_TYPE_SAMPLER_2D) {};
			~Sampler2D() {};
		};
	};
};