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

				//Cleansup
				void Shutdown();

				//Inits the Vk Allocator
				bool InitVKMemoryAllocator();

			public:
				//Creates a new store manually - eg: At init time, we may want to 
				//create a store which holds GPU data that lasts throughout the entire life
				//of the app etc
				//
				//returns a pointer to the store
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

			public:
				//Pass a rendering resource to be allocated automatically
				bool AllocateResourceAuto(EngineAPI::Graphics::RenderDevice* renderingDevice, 
					EngineAPI::Rendering::Resource* resource);

				//Allocates a resource in to a given store
				bool AllocateResourceToStore(EngineAPI::Graphics::RenderDevice* renderingDevice,
					EngineAPI::Graphics::DeviceMemoryStore* store,
					const VkMemoryRequirements& resourceMemoryRequriments,
					EngineAPI::Rendering::Resource* resource);

			protected:
				//Array of stores
				std::vector<EngineAPI::Graphics::DeviceMemoryStore> deviceStores;

			private:
				//Allocs a resource -> Will automatically find/create the store to use
				//for us. 
				bool AllocTextureResourceAuto(EngineAPI::Rendering::Resource* resource,
					RenderingResourceType resourceType,
					const VkMemoryRequirements& resourceMemoryRequirments,
					EngineAPI::Graphics::RenderDevice* renderingDevice,
					const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties);

				//Finds a store (if it already exists) to use when allocating a resource (AllocateResource()). 
				//Returns NULL if nothing could be found
				//
				//Won't look at private stores!
				EngineAPI::Graphics::DeviceMemoryStore* SearchExistingPublicMemoryStoresToUseForResource(
					const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties,
					const VkMemoryRequirements& resourceMemoryRequirments);

			private:
				//Searches for a memory type in memoryTypeBits (VkMemoryRequirements)
				//that includes all of the properties (requiredProperties)
				//
				//If thats not found, will search for a memory type index that supports
				//the properties defined by fallbackProperties (note: These can be 0!)
				bool FindMemoryTypeForProperties(uint32_t memoryTypeBits, 
					VkMemoryPropertyFlags requiredProperties, VkMemoryPropertyFlags fallbackProperties, 
					const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties,
					uint32_t* memTypeIndexOut);
			};
		}
	}
}