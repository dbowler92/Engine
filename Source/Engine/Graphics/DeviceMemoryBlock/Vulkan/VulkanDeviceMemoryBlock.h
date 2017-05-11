//VulkanDeviceMemoryBlock.h
//Created 04/05/17
//Created By Daniel Bowler
//
//Represents an individual piece of device memory (eg a VKImage, VkBuffer etc)

#pragma once

//Vulkan header
#include <vulkan\vulkan.h>

//Globals
#include "../../../Main/EngineStd.h"

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Stores pointer to the actual VK resource
#include "../../../Rendering/Resource/Resource.h"

//Forward declarations
namespace EngineAPI
{
	namespace Graphics
	{
		class DeviceMemoryStore;

		namespace Platform
		{
			class VulkanDeviceMemoryStore;
		}
	};
};

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanDeviceMemoryBlock
			{
				//Memory store can access + update private vars
				friend EngineAPI::Graphics::Platform::VulkanDeviceMemoryStore;

			public:
				VulkanDeviceMemoryBlock() {};
				virtual ~VulkanDeviceMemoryBlock() = 0 {};

				//Called when the app closes. 
				void Shutdown();

				//Init block data
				bool InitVKMemoryBlock(EngineAPI::Graphics::DeviceMemoryStore* parentStore, 
					EngineAPI::Rendering::Resource* resource,
					VkDeviceSize memoryAlignmentRequirments,
					VkDeviceSize memoryBlockSize,
					VkDeviceSize memoryBlockOffset, 
					VkDeviceSize resourceSize,
					bool isBlockMappable);

				//Frees the block -> Allowing it to be reused by other resources
				//
				//TODO: Delete the actual resource? 
				void FreeMemoryBlock();

			public:
				//Getters
				EngineAPI::Graphics::DeviceMemoryStore* GetParentStore() { return parentStore; };

				VkDeviceSize GetBlockSizeBytes() { return blockSizeBytes; };
				VkDeviceSize GetBlockOffsetInStoreBytes() { return blockOffsetInStoreBytes; };
				VkDeviceSize GetBlockAlignedOffsetInStoreBytes() { return blockAlignedOffsetInStoreBytes; };

				bool IsBlockFree() { return isFree; };
				bool IsBlockMappable() { return isMappable; };
				bool IsBlockCurrentlyMapped() { return isMapped; };

				void* GetBlockHostMemoryPointer() { return hostBlockPtr; };
				EngineAPI::Rendering::Resource* GetResource() { return resourcePtr; };

				VkDeviceSize GetResourceSize() { return resourceSizeBytes; };
				VkDeviceSize GetResourceAlignment() { return resourceAlignment; };

			protected:
				//Vulkan handle of the device memory this block resides in
				VkDeviceMemory parentVKDeviceMemory = VK_NULL_HANDLE;

			protected:
				//Store that owns this block
				EngineAPI::Graphics::DeviceMemoryStore* parentStore = nullptr;

				//Pointer to the resource that this store represents
				EngineAPI::Rendering::Resource* resourcePtr = nullptr;

				//Size of the resource as decided by the Vulkan API
				VkDeviceSize resourceSizeBytes = 0;

				//Block size in bytes
				VkDeviceSize blockSizeBytes = 0;

				//Resource alignment 
				VkDeviceSize resourceAlignment = 0;

				//Offset in to the memory store to the start of this block (bytes)
				VkDeviceSize blockOffsetInStoreBytes = 0;

				//Offset in to the memory store to the start of this block taking in to 
				//acount memory alignment requirments
				VkDeviceSize blockAlignedOffsetInStoreBytes = 0;

				//TODO: Memory block state flags
				bool isFree = true;               //Available to be used to store data (eg: Buffer data, texture data etc)
				bool isMappable = false;		  //Can we map the memory and write/read from it? Depends on the parent store or flags set when allocated in D3D11
				bool isMapped = false;			  //True when we are writing to this block from the CPU

				//If the block is mappable, this pointer can be used
				//to update the memory block
				void* hostBlockPtr = nullptr;
			};
		};
	};
};