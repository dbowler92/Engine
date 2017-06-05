//DescriptorPool.h
//Created 05/06/17
//Created By Daniel Bowler
//
//Efficient way for descriptors to be allocated

#pragma once

//Platform
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanDescriptorPool.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class DescriptorPool : public RENDERING_PLATFORM_IMPLEMENTATION(DescriptorPool)
		{
		public:
			DescriptorPool() {};
			virtual ~DescriptorPool() {};

		};
	};
};
