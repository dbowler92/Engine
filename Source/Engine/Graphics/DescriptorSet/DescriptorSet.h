//DescriptorSet.h
//Created 05/06/17
//Created By Daniel Bowler
//
//Represents the method to associate textures, constant buffers, etc
//with shader programs. 

#pragma once

//Platform
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanDescriptorSet.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class DescriptorSet : public RENDERING_PLATFORM_IMPLEMENTATION(DescriptorSet)
		{
		public:
			DescriptorSet() {};
			virtual ~DescriptorSet() {};

		};
	};
};
