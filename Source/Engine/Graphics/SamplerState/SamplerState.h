//SamplerState.h
//Created 08/06/17
//Created By Daniel Bowler
//
//Info on how to sample a texture in a shader. Eg: Address modes etc

#pragma once

//Platform
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanSamplerState.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class SamplerState : public RENDERING_PLATFORM_IMPLEMENTATION(SamplerState)
		{
		public:
			SamplerState() {};
			~SamplerState() {};
		};
	};
};
