//DeviceMemoryAllocator.h
//Created 04/05/17
//Created By Daniel Bowler
//
//Handles memory management of GPU resources per API

#pragma once

//Platform info
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanDeviceMemoryAllocator.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class DeviceMemoryAllocator : public RENDERING_PLATFORM_IMPLEMENTATION(DeviceMemoryAllocator)
		{
		public:
			DeviceMemoryAllocator() {};
			~DeviceMemoryAllocator() {};
		};
	};
};