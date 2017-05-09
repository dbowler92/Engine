#include "VulkanDeviceMemoryAllocator.h"

//Resources
#include "../../../Rendering/DepthTexture/DepthTexture.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanDeviceMemoryAllocator::Shutdown()
{
	//Cleanup all stores
	std::vector<EngineAPI::Graphics::DeviceMemoryStore>::iterator it = deviceStores.begin();
	while (it != deviceStores.end())
	{
		it->Shutdown();

		//Next store
		++it;
	}
	deviceStores.clear();
}

bool VulkanDeviceMemoryAllocator::InitVKMemoryAllocator()
{
	return true;
}

EngineAPI::Graphics::DeviceMemoryStore* VulkanDeviceMemoryAllocator::CreateNewMemoryStore(EngineAPI::Graphics::RenderDevice* renderDevice,
	VkDeviceSize deviceMemorySizeInBytesToAlloc, uint32_t memoryTypeIndex, bool isPublicStore)
{
	EngineAPI::Graphics::DeviceMemoryStore* storeOut = nullptr;

	VkDevice logicalDevice = renderDevice->GetVKLogicalDevice();
	VkPhysicalDeviceMemoryProperties memProperties = renderDevice->GetVKPhysicalDeviceMemoryProperties();

	EngineAPI::Graphics::DeviceMemoryStore newStore;
	if (!newStore.InitVKDeviceMemoryStore(&logicalDevice, deviceMemorySizeInBytesToAlloc, &memProperties, memoryTypeIndex, isPublicStore))
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryAllocator::CreateNewMemoryStore(): Could not create memory store!\n");
		return nullptr;
	}

	//Add to array of stores
	deviceStores.push_back(newStore);

	//Output
	storeOut = &deviceStores[deviceStores.size() - 1];
	return storeOut;
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

			//Allocate on the device
			success = AllocTextureResourceAuto(resource, resourceType, textureMemoryRequirments,
				renderingDevice, physicalDeviceMemoryProperties);
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

bool VulkanDeviceMemoryAllocator::AllocateResourceToStore(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Graphics::DeviceMemoryStore* store,
	const VkMemoryRequirements& resourceMemoryRequriments,
	EngineAPI::Rendering::Resource* resource)
{
	//Allocate in to the given store
	if (!store->SubAllocMemoryBlock(resourceMemoryRequriments.size,
		resourceMemoryRequriments.alignment, resource))
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryAllocator::AllocateResourceToStore() - Error: Could not alloc in to passed store\n");
		return false;
	}

	return true;
}

bool VulkanDeviceMemoryAllocator::AllocTextureResourceAuto(EngineAPI::Rendering::Resource* resource,
	RenderingResourceType resourceType,
	const VkMemoryRequirements& resourceMemoryRequirments,
	EngineAPI::Graphics::RenderDevice* renderingDevice,
	const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties)
{
	//Cast from resource to Texture (base class for depth texture, Texture2D etc etc etc)
	EngineAPI::Rendering::Texture* texture = static_cast<EngineAPI::Rendering::Texture*>(resource);
	VkImageUsageFlags textureUsage = texture->GetVKImageUsageFlags();
	
	//Work out the memoryIndex that is ideal for this type of resource. 
	

	//Get the memory store that we can use for this texture. If this returns
	//NULL, no store could be found. As a result, we will have to create a brand
	//new store
	EngineAPI::Graphics::DeviceMemoryStore* memStore = SearchExistingPublicMemoryStoresToUseForResource(physicalDeviceMemoryProperties, resourceMemoryRequirments);
	if (memStore)
	{
		//Sub alloc in to this store.
		if (!memStore->SubAllocMemoryBlock(resourceMemoryRequirments.size, 
			resourceMemoryRequirments.alignment, resource))
		{
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryAllocator::AllocTextureResourceAuto(): Could not alloc texture\n");
			return false;
		}
	}
	else
	{
		//Create new store for this memory
		EngineAPI::Graphics::DeviceMemoryStore newMemStore;

	}

	//Done
	return true;
}


EngineAPI::Graphics::DeviceMemoryStore* VulkanDeviceMemoryAllocator::SearchExistingPublicMemoryStoresToUseForResource(
	const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties,
	const VkMemoryRequirements& resourceMemoryRequirments)
{
	//Memory type index we want for this resource?
	uint32_t memoryTypeIndexForResource = 0;
	bool didFind = FindMemoryTypeForProperties(resourceMemoryRequirments.memoryTypeBits,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0,
		physicalDeviceMemoryProperties, &memoryTypeIndexForResource);

	if (!didFind)
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryAllocator::SearchExistingPublicMemoryStoresToUseForResource(): Error - Couldnt find a memory type index for this resource\n");
		return false;
	}

	//Does a store already exist for this resource with enough space left.
	std::vector<EngineAPI::Graphics::DeviceMemoryStore>::iterator it = deviceStores.begin();
	while (it != deviceStores.end())
	{
		uint32_t storeTypeIDX = it->GetVKMemoryTypeIndex();
		bool storePublicFlag = it->IsPublicMemoryStore();
		
		//Is this store public?
		if (storePublicFlag)
		{
			//Yes, is this store compatible with the requirements for the resource?
			bool isStoreCompatable = false;

			if (isStoreCompatable)
			{
				//Do we have enough space left in this store to suballoc in to?
				bool doesStoreHaveEnoughSpace = false;
				if (doesStoreHaveEnoughSpace)
				{
					//Can suballoc here. Return it
					return &(*it);
				}
			}

		}

		//Next store
		++it;
	}

	return nullptr;
}

bool VulkanDeviceMemoryAllocator::FindMemoryTypeForProperties(uint32_t memoryTypeBits, 
	VkMemoryPropertyFlags requiredProperties, VkMemoryPropertyFlags fallbackProperties,
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
			((physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & requiredProperties) == requiredProperties))
		{
			*memTypeIndexOut = i;
			return true;
		}
	}

	//Fallback:
	//
	//TODO
	for (int32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; ++i)
	{
		if ((memoryTypeBits & (1 << i)) &&
			((physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & fallbackProperties) == fallbackProperties))
		{
			*memTypeIndexOut = i;
			return true;
		}
	}

	//Failed.
	*memTypeIndexOut = 0;
	return false;
}