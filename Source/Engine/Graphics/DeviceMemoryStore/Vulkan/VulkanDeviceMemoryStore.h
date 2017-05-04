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
					uint32_t memoryTypeIndex);    //Index in to VkPhysicalDeviceMemoryProperties::memoryTypes[]

			public:

			protected:
				//Handle to the VK memory store
				VkDeviceMemory vkMemoryStoreHandle = VK_NULL_HANDLE;

				//Cached logical device - the 'owner' of this store
				VkDevice cachedVkLogicalDevice = VK_NULL_HANDLE;

				//Size of the memory store in bytes
				VkDeviceSize memoryStoreSizeBytes = 0;

				//Used when suballocating from this store. For the time being, everytime we suballoc
				//from the block, we will move the offset along (move the 'pointer' from where
				//we can suballoc more memory) - no defraging, no resizing etc. 
				//
				//Must be < memoryStoreSizeBytes (otherwise we would be suballocating from 
				//outside the memory block)
				VkDeviceSize suballocMemoryOffset = 0;
			};
		};
	};
};