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

//Allocation info
enum DeviceAllocationResourceType
{
	DEVICE_ALLOCATION_RESOURCE_TYPE_TEXTURE,
	DEVICE_ALLOCATION_RESOURCE_TYPE_BUFFER,
	DEVICE_ALLOCATION_RESOURCE_TYPE_RENDER_TARGET
};

typedef enum DeviceAllocationFlagsBits
{
	DEVICE_ALLOCATION_FLAG_PERMENENT_STORE_FLAG = 0x0000001,
} DeviceAllocationFlagsBits;
typedef EFlag DeviceAllocationFlag;

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

				//Override shutdown function
				void Shutdown();

				//Override init function
				bool Init();

			public:
				//Creates a brand new store for a given purpose
				//virtual bool AllocMemoryStore(DeviceStoreType storeRole, EUINT_64 storeSizeBytes) = 0;

			protected:
				//Array of stores
				std::vector<EngineAPI::Graphics::DeviceMemoryStore> deviceStores;
			};
		}
	}
}