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

//Uses std::vector
#include <vector>

//Manages a set of memory blocks within the store
#include "../../DeviceMemoryBlock/DeviceMemoryBlock.h"

//Vulkan header
#include <vulkan\vulkan.h>

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

				//Cleansup - frees all blocks from the store
				void Shutdown();

			public:
				//Vulkan init. Note: Vulkan implementation will automatically allocate aligned memory which
				//meets the minimum spec.
				bool InitVKDeviceMemoryStore(VkDevice* logicalDevice,
					VkDeviceSize deviceMemorySizeInBytesToAlloc,
					VkPhysicalDeviceMemoryProperties* fullDeviceMemoryProperties,
					uint32_t memoryTypeIndex);   //Index in to VkPhysicalDeviceMemoryProperties::memoryTypes[]
			
			public:
				//Sub alloc and free
				bool SubAllocMemoryBlock(VkDeviceSize blockSize, EngineAPI::Graphics::DeviceMemoryBlock& allocatedBlockOut);
				void FreeBlock(const EngineAPI::Graphics::DeviceMemoryBlock* block);

			public:
				//Getters
				VkDeviceMemory GetVKDeviceMemoryHandle() { return vkMemoryStoreHandle; };
				VkDevice GetOwningVKLogicalDevice() { return cachedVkLogicalDevice; };
				uint32_t GetVKMemoryTypeIndex() { return vkMemoryTypeIndex; };
				VkBool32 IsVKMemoryMappable() { return vkIsStoreMemoryMappable; };
				VkDeviceSize GetMemoryStoreSizeBytes() { return memoryStoreSizeBytes; };
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
				VkDeviceSize memoryStoreSizeBytes = 0;

				//Pointer to the begining of the store if host visible memory
				void* hostStorePtr = nullptr;

				//Array of blocks - sub allocations from within this block. TODO: Custom 
				//resizing array. 
				std::vector<EngineAPI::Graphics::DeviceMemoryBlock> memoryBlocksArray;
			};
		};
	};
};