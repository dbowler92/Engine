//VulkanDeviceMemoryStore.h
//Created 28/04/17
//Created By Daniel Bowler
//
//Represents a store of device memory (GPU local or, perhaps, host local && GPU visible). Vulkan provides the application developer
//the ability to manage GPU memory (In, say, D3D11, this was handled by the driver). This class
//will represent a single store of memory for use in the application (Eg: A memory store could be 
//allocated for data that persists throughout the entire game. Another store for
//textures, another for buffers and another for short term allocations)
//
//TODO: Maybe subject of my MSc dissertation. Failing that, this is a big topic which
//could be a lot of fun to explore!

#pragma once

//Globals
#include "../../../Main/EngineStd.h"

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Uses std::vector
#include <vector>

//Manages a set of memory blocks within the store
#include "../../DeviceMemoryBlock/DeviceMemoryBlock.h"

//Vulkan header
#include <vulkan\vulkan.h>

//Type of store
enum DeviceStoreType
{
	DEVICE_STORE_TYPE_GPU_ONLY_ALLOCATIONS,
	DEVICE_STORE_TYPE_CPU_MAPPABLE_ALLOCATIONS
};

//Creation info for the stores
enum DeviceStoreCreateFlags
{

};

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanDeviceMemoryStore
			{
			public:
				VulkanDeviceMemoryStore() {};
				virtual ~VulkanDeviceMemoryStore() = 0 {};

				//Override shutdown
				void Shutdown();

			public:
				//Vulkan init. Note: Vulkan implementation will automatically allocate aligned memory which
				//meets the minimum spec.
				bool InitVKDeviceMemoryStore(VkDevice* logicalDevice,
					VkDeviceSize deviceMemorySizeInBytesToAlloc,
					VkPhysicalDeviceMemoryProperties* fullDeviceMemoryProperties,
					uint32_t memoryTypeIndex);   //Index in to VkPhysicalDeviceMemoryProperties::memoryTypes[]
			
			public:
				//Override alloc/dealloc functions
				bool SubAllocMemoryBlock(EUINT_64 blockSize, EngineAPI::Graphics::DeviceMemoryBlock& allocatedBlockOut);
				void DeallocBlock(const EngineAPI::Graphics::DeviceMemoryBlock* block);

			public:
				//Getters
				VkDeviceMemory GetVKDeviceMemoryHandle() { return vkMemoryStoreHandle; };
				VkDevice GetOwningVKLogicalDevice() { return cachedVkLogicalDevice; };
				uint32_t GetVKMemoryTypeIndex() { return vkMemoryTypeIndex; };
				VkBool32 IsVKMemoryMappable() { return vkIsStoreMemoryMappable; };
				EUINT_64 GetMemoryStoreSizeBytes() { return memoryStoreSizeBytes; };
				std::vector<EngineAPI::Graphics::DeviceMemoryBlock>* GetMemoryBlocksArray() { return &memoryBlocksArray; };

			protected:
				//Handle to the VK memory store
				VkDeviceMemory vkMemoryStoreHandle = VK_NULL_HANDLE;

				//Vulkan memory type index for this store
				uint32_t vkMemoryTypeIndex;

				//Vulkan memory property flags
				VkMemoryPropertyFlags vkMemoryPropertyFlags;

				//Is the store mappable?
				VkBool32 vkIsStoreMemoryMappable = false;

				//Cached logical device - the 'owner' of this store
				VkDevice cachedVkLogicalDevice = VK_NULL_HANDLE;

			protected:
				//Size of the store in bytes
				EUINT_64 memoryStoreSizeBytes = 0;

				//Pointer to the begining of the store if host visible memory
				void* hostStorePtr = nullptr;

				//Array of blocks - sub allocations from within this block. TODO: Custom 
				//resizing array. 
				std::vector<EngineAPI::Graphics::DeviceMemoryBlock> memoryBlocksArray;
			};
		};
	};
};