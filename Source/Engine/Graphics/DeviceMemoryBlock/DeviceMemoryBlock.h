//DeviceMemoryBlock.h
//Created 04/05/17
//Created By Daniel Bowler
//
//Represents an individual piece of device memory. The DeviceMemoryStore will contain
//a set of these blocks. 

#pragma once

//Platform info
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanDeviceMemoryBlock.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class DeviceMemoryBlock : public RENDERING_PLATFORM_IMPLEMENTATION(DeviceMemoryBlock)
		{
		public:
			DeviceMemoryBlock() {};
			~DeviceMemoryBlock() {};
		};
	};
};