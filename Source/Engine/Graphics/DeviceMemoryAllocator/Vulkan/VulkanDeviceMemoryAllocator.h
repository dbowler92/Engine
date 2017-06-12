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

//Forward declarations
namespace EngineAPI
{
	namespace Graphics
	{
		class RenderDevice;
	};
};

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

				//Cleansup at app shutdown time. 
				void Shutdown();

				//Inits the Vk Allocator
				bool InitVKMemoryAllocator(EngineAPI::Graphics::RenderDevice* renderingDevice);

			public:
				//Debug: 
				//
				//Writes to a text file the full state of the memory allocator (File extention is .DUMP. It is, however, a txt file)
				void Debug_LongDump(std::string filename);

			public:
				//Creates a new store manually - eg: At init time, we may want to 
				//create a store which holds GPU data that lasts throughout the entire life
				//of the app etc
				//
				//Note: bool isPublicStore => If true, this store can be used to alloc resources
				//during calls to AllocResourceAuto(). If false, you must use AllocateResourceToStore() and
				//pass this store as a param. 
				//
				//Use of private/non public stores include: Global data that you init once, 
				//render target stores that you perhaps want right at the start of the
				//device memory etc
				EngineAPI::Graphics::DeviceMemoryStore* CreateNewMemoryStore(EngineAPI::Graphics::RenderDevice* renderDevice,
					VkDeviceSize deviceMemorySizeInBytesToAlloc, uint32_t memoryTypeIndex, bool isPublicStore);

				//
				//TODO: Destroys a store
				//

			public:
				//Pass a rendering resource to be allocated automatically
				SuballocationResult AllocateResourceAuto(EngineAPI::Graphics::RenderDevice* renderingDevice,
					EngineAPI::Rendering::Resource* resource);

				//Allocates a resource in to a given store
				SuballocationResult AllocateResourceToStore(EngineAPI::Graphics::RenderDevice* renderingDevice,
					EngineAPI::Graphics::DeviceMemoryStore* store,
					const VkMemoryRequirements& resourceMemoryRequriments,
					EngineAPI::Rendering::Resource* resource);

			protected:
				//Array of stores
				EngineAPI::Graphics::DeviceMemoryStore deviceMemoryStoresArray[ENGINE_CONFIG_VULKAN_API_MAX_NUMBER_OF_MEMORY_STORES];
				uint32_t deviceMemoryStoresCount = 0; //Number created

				//Device memory properties - cached at init time
				VkPhysicalDeviceMemoryProperties cachedVKDeviceMemoryProperties;

			private:
				//Allocs a resource -> Will automatically find/create the store to use
				//for us. 
				SuballocationResult AllocResourceAuto(EngineAPI::Graphics::RenderDevice* renderingDevice,
					EngineAPI::Rendering::Resource* resource,
					RenderingResourceType resourceType,
					VkMemoryPropertyFlags resourceMemoryPropertyOptimalFlags, 
					VkMemoryPropertyFlags resourceMemoryPropertyFallbackFlags,
					bool isGPUOnlyStorage,
					const VkMemoryRequirements& resourceMemoryRequirments,
					const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties);

				
			private:
				//Finds a store (if it already exists) to use when allocating a resource (AllocateResource()). 
				//Returns NULL if nothing could be found
				//
				//Won't look at private stores!
				EngineAPI::Graphics::DeviceMemoryStore* SearchExistingPublicMemoryStoresToUseForResource(
					VkMemoryPropertyFlags resourceMemoryPropertyOptimalFlags,
					VkMemoryPropertyFlags resourceMemoryPropertyFallbackFlags,
					const VkMemoryRequirements& resourceMemoryRequirments,
					const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties);
			};
		}
	}
}