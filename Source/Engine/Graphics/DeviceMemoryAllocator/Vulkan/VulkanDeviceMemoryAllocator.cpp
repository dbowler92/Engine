#include "VulkanDeviceMemoryAllocator.h"

//Resources
#include "../../../Rendering/DepthTexture/DepthTexture.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanDeviceMemoryAllocator::Shutdown()
{}

bool VulkanDeviceMemoryAllocator::InitVKMemoryAllocator()
{
	return true;
}

bool VulkanDeviceMemoryAllocator::AllocateResource(EngineAPI::Rendering::Resource* resource, 
	EngineAPI::Graphics::RenderDevice* renderingDevice)
{
	//Success when allocing memory?
	bool success = false;

	//Alloc based on resource type
	RenderingResourceType resourceType = resource->GetResourceType();
	switch (resourceType)
	{
		case RENDERING_RESOURCE_TYPE_TEXTURE:
		{
			EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanDeviceMemoryAllocator::AllocateResource(): Allocating Texture\n");
			
			success = true;
			break;
		}
		case RENDERING_RESOURCR_TYPE_DEPTH_TEXTURE:
		{
			EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanDeviceMemoryAllocator::AllocateResource(): Allocating Depth Texture\n");
			
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
			success = AllocTextureResource(resource, resourceType, 
				renderingDevice, textureMemoryRequirments, physicalDeviceMemoryProperties);
			break;
		}
		case RENDERING_RESOURCE_TYPE_RENDER_TARGET_TEXTURE:
		{
			EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanDeviceMemoryAllocator::AllocateResource(): Allocating Render Target\n");
			
			success = true;
			break;
		}
		case RENDERING_RESOURCE_TYPE_BUFFER:
		{
			EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanDeviceMemoryAllocator::AllocateResource(): Allocating Buffer\n");
			
			success = true;
			break;
		}
		default:
		{	
			//Error
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryAllocator::AllocateResource(): Error - Unknown resource type\n");
			
			success = false;
			break;
		}
	}

	//Done
	return success;
}

bool VulkanDeviceMemoryAllocator::AllocTextureResource(EngineAPI::Rendering::Resource* resource,
	RenderingResourceType resourceType,
	EngineAPI::Graphics::RenderDevice* renderingDevice,
	const VkMemoryRequirements& resourceMemoryRequirments,
	const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties)
{
	//Work out the memoryIndex that is ideal for this type of resource. 

	//Does a store of this type exist already?

	//Get all stores of this type

	//Find one which has a block available that can fit this resource

	//Failed as no store can fit this image. 

	//Create a brand new store

	//Alloc in the correct store

	//Done
	//Else, no store of this type exists

	//Create new one for this type

	//Alloc in this new store

	//Done
	//Done

	//Done
	return true;
}


EngineAPI::Graphics::DeviceMemoryStore* VulkanDeviceMemoryAllocator::SearchExistingMemoryStoresToUseForResource(EngineAPI::Rendering::Resource* resource)
{
	return nullptr;
}