//DescriptorBinding.h
//Created 05/06/17
//Created By Daniel Bowler
//
//Descriptor sets contains an array of bindings. Each describes how one
//resource will be mapped to shaders. 

#pragma once

//Platform
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanDescriptorBinding.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class DescriptorBinding : public RENDERING_PLATFORM_IMPLEMENTATION(DescriptorBinding)
		{
		public:
			DescriptorBinding() {};
			virtual ~DescriptorBinding() {};

		};
	};
};
