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

//Base
#include "../Common/CommonDeviceMemoryStore.h"

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
			class VulkanDeviceMemoryStore : public EngineAPI::Graphics::Interface::CommonDeviceMemoryStore
			{
			public:
				VulkanDeviceMemoryStore() {};
				virtual ~VulkanDeviceMemoryStore() = 0 {};

				//Override shutdown
				void Shutdown() override;

			public:
				//Vulkan init. Note: Vulkan implementation will automatically allocate aligned memory which
				//meets the minimum spec.
				bool InitVKDeviceMemoryStore(VkDevice* logicalDevice,
					VkDeviceSize deviceMemorySizeInBytesToAlloc,
					VkPhysicalDeviceMemoryProperties* fullDeviceMemoryProperties,
					uint32_t memoryTypeIndex);   //Index in to VkPhysicalDeviceMemoryProperties::memoryTypes[]
			
			public:
				//Override alloc/dealloc functions
				bool SubAllocMemoryBlock(EUINT_64 blockSize, EngineAPI::Graphics::DeviceMemoryBlock& allocatedBlockOut) override;
				void DeallocBlock(const EngineAPI::Graphics::DeviceMemoryBlock* block) override;

			public:
				//Getters
				VkDeviceMemory GetVKDeviceMemoryHandle() { return vkMemoryStoreHandle; };
				VkDevice GetOwningVKLogicalDevice() { return cachedVkLogicalDevice; };
				uint32_t GetVKMemoryTypeIndex() { return vkMemoryTypeIndex; };
				VkBool32 IsVKMemoryMappable() { return vkIsStoreMemoryMappable; };

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
			};
		};
	};
};