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
			public:
				VulkanDeviceMemoryBlock() {};
				virtual ~VulkanDeviceMemoryBlock() = 0 {};

				//Cleansup this block
				void Shutdown();

				//Init block data
				bool InitVKMemoryBlock(EngineAPI::Graphics::DeviceMemoryStore* parentStore, 
					EngineAPI::Rendering::Resource* resource, 
					VkDeviceSize memoryBlockSize,
					VkDeviceSize memoryBlockOffset, bool isBlockMappable);

			public:
				//Getters
				VkDeviceSize GetBlockSizeBytes() { return blockSizeBytes; };
				VkDeviceSize GetBlockOffsetInStoreBytes() { return blockOffsetInStoreBytes; };

				bool IsBlockFree() { return isFree; };
				bool IsBlockMappable() { return isMappable; };
				bool IsBlockCurrentlyMapped() { return isMapped; };

				void* GetBlockHostMemoryPointer() { return hostBlockPtr; };

			protected:
				//Vulkan handle of the device memory this block resides in
				VkDeviceMemory parentVKDeviceMemory = VK_NULL_HANDLE;

			protected:
				//Store that owns this block
				EngineAPI::Graphics::DeviceMemoryStore* parentStore = nullptr;

				//Pointer to the resource that this store represents
				EngineAPI::Rendering::Resource* resourcePtr = nullptr;

				//Block size in bytes
				VkDeviceSize blockSizeBytes = 0;

				//Offset in to the memory store to the start of this block (bytes)
				VkDeviceSize blockOffsetInStoreBytes = 0;

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