//PushConstantBinding.h
//Created 08/06/17
//Created By Daniel Bowler
//
//Efficient method of updating constants in shaders. 

#pragma once

//Platform
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan/VulkanPushConstantBinding.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class PushConstantBinding : public RENDERING_PLATFORM_IMPLEMENTATION(PushConstantBinding)
		{
		public:
			PushConstantBinding() {};
			virtual ~PushConstantBinding() {};

		};
	};
};


