//VulkanDeviceMemoryAllocator.h
//Created 04/05/17
//Created By Daniel Bowler
//
//Handles memory management of GPU resources for Vulkan.

#pragma once

//Globals
#include "../../../Main/EngineStd.h"

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Contains a list of memory stores
#include "../../DeviceMemoryStore/DeviceMemoryStore.h"

//Resource - passed when allocating. This is the base class of texture objects
//and buffers. 
#include "../../../Rendering/Resource/Resource.h"

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanDeviceMemoryAllocator
			{
			public:
				VulkanDeviceMemoryAllocator() {};
				virtual ~VulkanDeviceMemoryAllocator() = 0 {};

				//Cleansup
				void Shutdown();

				//Inits the Vk Allocator
				bool InitVKMemoryAllocator();

			public:
				//Pass a rendering resource to be allocated
				//bool AllocateResource(EngineAPI::Rendering::Resource* resource);

			protected:
				//Array of stores
				std::vector<EngineAPI::Graphics::DeviceMemoryStore> deviceStores;
			};
		}
	}
}