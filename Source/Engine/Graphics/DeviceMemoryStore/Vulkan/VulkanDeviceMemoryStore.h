//VulkanDeviceMemoryStore.h
//Created 28/04/17
//Created By Daniel Bowler
//
//Represents a store of device memory (GPU local or, perhaps, host local && GPU visible). Vulkan provides the application developer
//the ability to manage GPU memory (In, say, D3D11, this was handled by the driver). This class
//will represent a single store of memory for use in the application. We can then suballocate
//a block from this store when needed. Note: You may have several stores each of which support
//the same memory index -> For example, when we run out of space for GPU only allocations, a new
//store will be allocated that handle GPU only allocs. 
//
//TODO: Maybe subject of my MSc dissertation. Failing that, this is a big topic which
//could be a lot of fun to explore!

#pragma once

//Globals
#include "../../../Main/EngineStd.h"

//Debug
#include "../../../Debug/Log/DebugLog.h"

//STL
#include <list>
#include <vector>

//Manages a set of memory blocks within the store
#include "../../DeviceMemoryBlock/DeviceMemoryBlock.h"

//Base class for resources
#include "../../../Rendering/Resource/Resource.h"

//Vulkan header
#include <vulkan\vulkan.h>

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanDeviceMemoryAllocator;
		}

		class DeviceMemoryAllocator;
	}
}

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanDeviceMemoryStore
			{
				//Allocator can access private functions
				friend EngineAPI::Graphics::Platform::VulkanDeviceMemoryAllocator;

			public:
				VulkanDeviceMemoryStore() {};
				virtual ~VulkanDeviceMemoryStore() = 0 {};

				//Called at shutdown - cleans up the memory this store
				//manages
				//
				//FreeStore() will be used to reset an entire memory store but
				//not destroy the vkDeviceMemory object (thus, allowing us
				//to re-suballoc out of this store)
				void Shutdown();

			public:
				//Vulkan init. Note: Vulkan implementation will automatically allocate aligned memory which
				//meets the minimum spec.
				bool InitVKDeviceMemoryStore(EngineAPI::Graphics::DeviceMemoryAllocator* parentAllocator,
					VkDevice* logicalDevice,
					VkDeviceSize deviceMemorySizeInBytesToAlloc,
					VkPhysicalDeviceMemoryProperties* fullDeviceMemoryProperties,
					uint32_t memoryTypeIndex,    //Index in to VkPhysicalDeviceMemoryProperties::memoryTypes[]
					bool isPublicMemoryStore);

			public:
				//Getters
				EngineAPI::Graphics::DeviceMemoryAllocator* GetParentAllocator() { return parentMemoryAllocator; };

				VkDeviceMemory GetVKDeviceMemoryHandle() { return vkMemoryStoreHandle; };
				VkDevice GetOwningVKLogicalDevice() { return cachedVkLogicalDevice; };
				uint32_t GetVKMemoryTypeIndex() { return vkMemoryTypeIndex; };
				VkBool32 IsVKMemoryMappable() { return vkIsStoreMemoryMappable; };
				VkDeviceSize GetMemoryStoreSizeBytes() { return memoryStoreSizeBytes; };
				
				bool IsPublicMemoryStore() { return isPublicStore; };
				bool IsMemoryStoreActive() { return isStoreActive; };
				
				std::list<EngineAPI::Graphics::DeviceMemoryBlock>* GetMemoryBlocksList() { return &deviceMemoryBlocksList; };

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
				//Array of blocks - sub allocations from within this block. List and not
				//a vector (though a vector of pointers would work...) since we don't want
				//the objects within the array to be moved around in CPU memory => Blocks
				//have a pointer to a resource and said resource has a pointer back to 
				//the DeviceMemoryBlock -> The latter link is broken if the DeviceMemoryBlock
				//is copied to a new location in memory after a vector resize.
				std::list<EngineAPI::Graphics::DeviceMemoryBlock> deviceMemoryBlocksList;
				//std::vector<EngineAPI::Graphics::DeviceMemoryBlock*> deviceMemoryBlocksArray;

				//Pointer to the last suballoced block - Used to help quickly suballoc a new
				//block after it
				EngineAPI::Graphics::DeviceMemoryBlock* lastSuballocedBlock = nullptr;

				//Parent allocator
				EngineAPI::Graphics::DeviceMemoryAllocator* parentMemoryAllocator = nullptr;

				//Size of the store in bytes
				VkDeviceSize memoryStoreSizeBytes = 0;

				//Pointer to the beginning of the store if host visible memory
				void* hostStorePtr = nullptr;

				//Is this a 'public' store (Used during AllocResourceAuto() calls). If not, 
				//you can only suballoc blocks in this store by calls to AllocResourceToStore()
				bool isPublicStore = false;

				//Is the store active -> Available to be suballoced in to. 
				bool isStoreActive = false;

			private:
				//Loops through the list of blocks and tries to find one
				//which is a) free and b) large enough to hold our resource
				EngineAPI::Graphics::DeviceMemoryBlock* SearchExistingBlocksListToUseToSuballocResource(
					EngineAPI::Rendering::Resource* resource,
					VkDeviceSize blockSizeNeeded, VkDeviceSize resourceAlignment);

			private:
				//Allocator can access these functions
				//				
				//Suballoc and free
				bool Private_Suballoc(EngineAPI::Rendering::Resource* resource,
					VkDeviceSize blockSize, VkDeviceSize resourceAlignment);
				void Private_FreeBlock(EngineAPI::Graphics::DeviceMemoryBlock* block);
			};
		};
	};
};