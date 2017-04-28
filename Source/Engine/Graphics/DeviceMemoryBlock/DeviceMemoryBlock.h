//DeviceMemoryBlock.h
//Created 28/04/17
//Created By Daniel Bowler
//
//Represents the interface to interact with device/GPU memory. Vulkan/D3D12/Console APIs
//provide the app/engine developer more control over GPU memory. Therefore, we may want to 
//allocate large blocks of GPU memory (several blocks perhaps?_ and then suballocate as and when required. 
//
//D3D11/OpenGL hide the memory management from the app developer. Therefore, for those APIs, 
//this class won't do anything interesting - simply using the default API provided functions. For these
//APIs, you will likely have one device memory block instance. 
//
//That said, the aim is that higher level developers using this engine will use the same class (this)
//when allocating GPU resources between different graphics APIs - indeed, same functions. 

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
