#include "VulkanDeviceMemoryAllocator.h"

//Resources
#include "../../../Rendering/DepthTexture/DepthTexture.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanDeviceMemoryAllocator::Shutdown()
{
	//Cleanup all stores
	for (int i = 0; i < ENGINE_CONFIG_VULKAN_API_MAX_NUMBER_OF_MEMORY_STORES; i++)
		deviceMemoryStoresArray[i].Shutdown();
}

bool VulkanDeviceMemoryAllocator::InitVKMemoryAllocator()
{
	return true;
}

EngineAPI::Graphics::DeviceMemoryStore* VulkanDeviceMemoryAllocator::CreateNewMemoryStore(EngineAPI::Graphics::RenderDevice* renderDevice,
	VkDeviceSize deviceMemorySizeInBytesToAlloc, uint32_t memoryTypeIndex, bool isPublicStore)
{
	//Fail if we have reached the maximum allowed stores
	if (deviceMemoryStoresCount >= ENGINE_CONFIG_VULKAN_API_MAX_NUMBER_OF_MEMORY_STORES)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryAllocator::CreateNewMemoryStore() Error - Too many active stores!\n");
		return false;
	}

	VkDevice logicalDevice = renderDevice->GetVKLogicalDevice();
	VkPhysicalDeviceMemoryProperties memProperties = renderDevice->GetVKPhysicalDeviceMemoryProperties();

	//Init store
	if (!deviceMemoryStoresArray[deviceMemoryStoresCount].InitVKDeviceMemoryStore((DeviceMemoryAllocator*)this, &logicalDevice, deviceMemorySizeInBytesToAlloc, &memProperties, memoryTypeIndex, isPublicStore))
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryAllocator::CreateNewMemoryStore(): Could not init memory store!\n");
		return nullptr;
	}

	//New store, increment counter
	deviceMemoryStoresCount++;

	//Return pointer to this memory store
	return &(deviceMemoryStoresArray[deviceMemoryStoresCount - 1]);
}

bool VulkanDeviceMemoryAllocator::AllocateResourceToStore(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Graphics::DeviceMemoryStore* store,
	const VkMemoryRequirements& resourceMemoryRequriments,
	EngineAPI::Rendering::Resource* resource)
{
	//Allocate in to the given store
	if (!store->Private_Suballoc(resource, resourceMemoryRequriments.size, resourceMemoryRequriments.alignment))
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryAllocator::AllocateResourceToStore() - Error: Could not alloc in to passed store\n");
		return false;
	}

	return true;
}

bool VulkanDeviceMemoryAllocator::AllocateResourceAuto(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Rendering::Resource* resource)
{
	//Success when allocing memory?
	bool success = false;

	//Alloc based on resource type
	RenderingResourceType resourceType = resource->GetResourceType();
	switch (resourceType)
	{
		case RENDERING_RESOURCE_TYPE_TEXTURE:
		{
			EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanDeviceMemoryAllocator::AllocateResourceAuto(): Allocating Texture\n");
			
			success = true;
			break;
		}
		case RENDERING_RESOURCR_TYPE_DEPTH_TEXTURE:
		{
			EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanDeviceMemoryAllocator::AllocateResourceAuto(): Allocating Depth Texture\n");
			
			//DepthTexture inherits from Texture which inherits from Resource. Thus, cast
			//the resource pointer to depth texture (we can then get its memory
			//requirements, texture handle, etc etc etc)
			//
			//Due to Resource::GetType(), we know the type so an implicit (down) cast will done nicely
			EngineAPI::Rendering::DepthTexture* depthResource = static_cast<EngineAPI::Rendering::DepthTexture*>(resource);
			
			//Vulkan memory requirements for this texture -> Helps us find the correct store
			//for this resource. 
			VkMemoryRequirements textureMemoryRequirments = depthResource->GetVKImageMemoryRequirments();
			
			//Device memory properties. Used with VkMemoryRequirments to work out what memory type
			//we want to use when allocing this depth texture
			VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = renderingDevice->GetVKPhysicalDeviceMemoryProperties();

			//Memory properties for this resource
			VkMemoryPropertyFlags optimalFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			VkMemoryPropertyFlags fallbackFlags = 0;
			if (depthResource->IsTextureDynamic())
			{
				optimalFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
				fallbackFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
			}

			//Allocate on the device
			success = AllocTextureResourceAuto(renderingDevice, resource, resourceType, 
				optimalFlags, fallbackFlags,
				textureMemoryRequirments, physicalDeviceMemoryProperties);
			break;
		}
		case RENDERING_RESOURCE_TYPE_RENDER_TARGET_TEXTURE:
		{
			EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanDeviceMemoryAllocator::AllocateResourceAuto(): Allocating Render Target\n");
			
			success = true;
			break;
		}
		case RENDERING_RESOURCE_TYPE_BUFFER:
		{
			EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanDeviceMemoryAllocator::AllocateResourceAuto(): Allocating Buffer\n");
			
			success = true;
			break;
		}
		default:
		{	
			//Error
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryAllocator::AllocateResourceAuto(): Error - Unknown resource type\n");
			
			success = false;
			break;
		}
	}

	//Done
	return success;
}

bool VulkanDeviceMemoryAllocator::AllocTextureResourceAuto(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Rendering::Resource* resource,
	RenderingResourceType resourceType,
	VkMemoryPropertyFlags resourceMemoryPropertyOptimalFlags,
	VkMemoryPropertyFlags resourceMemoryPropertyFallbackFlags,
	const VkMemoryRequirements& resourceMemoryRequirments,
	const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties)
{
	//Cast from resource to Texture (base class for depth texture, Texture2D etc etc etc)
	EngineAPI::Rendering::Texture* texture = static_cast<EngineAPI::Rendering::Texture*>(resource);
	//VkImageUsageFlags textureUsage = texture->GetVKImageUsageFlags();

	//Get the memory store that we can use for this texture. If this returns
	//NULL, no store could be found. As a result, we will have to create a brand
	//new store
	EngineAPI::Graphics::DeviceMemoryStore* memStore = nullptr;
	memStore = SearchExistingPublicMemoryStoresToUseForResource(resourceMemoryPropertyOptimalFlags, resourceMemoryPropertyFallbackFlags,
			resourceMemoryRequirments, physicalDeviceMemoryProperties);
	if (memStore)
	{
		//Sub alloc in to this store.
		if (!memStore->Private_Suballoc(resource, resourceMemoryRequirments.size, resourceMemoryRequirments.alignment))
		{
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryAllocator::AllocTextureResourceAuto(): Could not alloc texture\n");
			return false;
		}
	}
	else
	{
		//Create new store for this memory
		EngineAPI::Graphics::DeviceMemoryStore* newMemStore;

	}

	//Done
	return true;
}


EngineAPI::Graphics::DeviceMemoryStore* VulkanDeviceMemoryAllocator::SearchExistingPublicMemoryStoresToUseForResource(
	VkMemoryPropertyFlags resourceMemoryPropertyOptimalFlags,
	VkMemoryPropertyFlags resourceMemoryPropertyFallbackFlags,
	const VkMemoryRequirements& resourceMemoryRequirments,
	const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties)
{
	//Memory type index we want for this resource?
	uint32_t memoryTypeIndexForResource = 0;
	bool didFind = FindMemoryTypeForProperties(resourceMemoryRequirments.memoryTypeBits,
		resourceMemoryPropertyOptimalFlags, physicalDeviceMemoryProperties, &memoryTypeIndexForResource);

	if (!didFind)
	{
		//Fallback - TODO: Make better. 
		bool didFindFallback = FindMemoryTypeForProperties(resourceMemoryRequirments.memoryTypeBits,
			resourceMemoryPropertyFallbackFlags, physicalDeviceMemoryProperties, &memoryTypeIndexForResource);

		if (!didFindFallback)
		{
			//Error
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryAllocator::SearchExistingPublicMemoryStoresToUseForResource()- Error: Could not find any memory types suitable for this resource\n");
			return false;
		}
	}

	//Does a store already exist for this resource with enough space left.
	for (int i = 0; i < deviceMemoryStoresCount; i++)
	{
		//Check if active
		if (!deviceMemoryStoresArray[i].IsMemoryStoreActive())
			break;

		//Only check for public stores
		bool storePublicFlag = deviceMemoryStoresArray[i].IsPublicMemoryStore();
		if (storePublicFlag)
		{
			//Yes. Is the stores memory type index the same as the one required by the
			//resource?
			uint32_t storeTypeIDX = deviceMemoryStoresArray[i].GetVKMemoryTypeIndex();
			bool isStoreCompatable = (storeTypeIDX == memoryTypeIndexForResource);
			if (isStoreCompatable)
			{
				//Yes! Is the stores max size > required bytes?
				VkDeviceSize storeSizeBytes = deviceMemoryStoresArray[i].GetMemoryStoreSizeBytes();
				if (storeSizeBytes > resourceMemoryRequirments.size) //TODO: Alignment?
				{
					//Yes! Does this store have enough space for us to alloc 
					//this resource in to? TODO: Take in to account alignment
					VkDeviceSize storeSizeRemaining = 0;
					bool doesStoreHaveEnoughSpace = false;
					if (doesStoreHaveEnoughSpace)
					{
						//Can suballoc here. Return it
						return &(deviceMemoryStoresArray[i]);
					}
				}
			}
		}
	}

	return nullptr;
}

bool VulkanDeviceMemoryAllocator::FindMemoryTypeForProperties(uint32_t memoryTypeBits, 
	VkMemoryPropertyFlags properties,
	const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties,
	uint32_t* memTypeIndexOut)
{
	//
	//See official docs: https://www.khronos.org/registry/vulkan/specs/1.0/man/html/VkPhysicalDeviceMemoryProperties.html
	//
	//Required:
	for (int32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; ++i)
	{
		if ((memoryTypeBits & (1 << i)) &&
			((physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties))
		{
			*memTypeIndexOut = i;
			return true;
		}
	}

	//Failed.
	*memTypeIndexOut = 0;
	return false;
}