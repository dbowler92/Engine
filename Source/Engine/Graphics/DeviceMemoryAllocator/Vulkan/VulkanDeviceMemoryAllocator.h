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
				//Pass a rendering resource to be allocated
				bool AllocateResource(EngineAPI::Rendering::Resource* resource, 
					EngineAPI::Graphics::RenderDevice* renderingDevice);

			protected:
				//Cache the rendering device used to create this allocator
				EngineAPI::Graphics::RenderDevice* cachedRenderingDevice;

				//Array of stores
				std::vector<EngineAPI::Graphics::DeviceMemoryStore> deviceStores;

			private:
				//Allocs a resource
				bool AllocTextureResource(EngineAPI::Rendering::Resource* resource,
					RenderingResourceType resourceType,
					EngineAPI::Graphics::RenderDevice* renderingDevice,
					const VkMemoryRequirements& resourceMemoryRequirments,
					const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties);

				//Finds a store (if it already exists) to use when allocating a resource (AllocateResource()). 
				//Returns NULL if nothing could be found
				EngineAPI::Graphics::DeviceMemoryStore* SearchExistingMemoryStoresToUseForResource(EngineAPI::Rendering::Resource* resource);
			};
		}
	}
}