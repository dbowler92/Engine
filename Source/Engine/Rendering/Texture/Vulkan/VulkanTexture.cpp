#include "VulkanTexture.h"

//Memory allocator
#include "../../../Graphics/DeviceMemoryAllocator/DeviceMemoryAllocator.h"

using namespace EngineAPI::Rendering::Platform;

bool VulkanTexture::InitVKTexture(EngineAPI::Graphics::RenderDevice* renderingDevice, 
	VkImageCreateInfo* imageCreateInfo, RenderingResourceUsage resourceUsage)
{
	//Cache texture info for future use
	vkImageTilingMode   = imageCreateInfo->tiling;
	vkTextureDimentions = imageCreateInfo->extent;
	mipLevelsCount = imageCreateInfo->mipLevels;
	vkTextureFormat = imageCreateInfo->format;
	vkImageType = imageCreateInfo->imageType;
	vkImageUsageFlags = imageCreateInfo->usage;
	vkSamplesCountFlag = imageCreateInfo->samples;
	//ETC

	//Texture usage
	this->resourceUsage = resourceUsage;

	//Get the logical device used to create this texture. 
	cachedVkDevice = renderingDevice->GetVKLogicalDevice();

	//Create logical image with passed in data
	VkResult result = vkCreateImage(cachedVkDevice, imageCreateInfo, nullptr, &vkImageHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanTextur::InitVKTexture() - Error: Could not create VkImage\n");
		return false;
	}

	//Cache memory requirements for this image
	vkGetImageMemoryRequirements(cachedVkDevice, vkImageHandle, &vkResourceMemoryRequirments);

	//Done
	return true;
}

bool VulkanTexture::AllocAndBindVKTextureMemory(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Graphics::DeviceMemoryStore* optionalMemoryStore)
{
	//Alloc memory for the texture on the device
	EngineAPI::Graphics::DeviceMemoryAllocator* memoryAllocator = renderingDevice->GetDeviceMemoryAllocator();
	if (optionalMemoryStore)
	{
		//Use passed in memory store
		SuballocationResult result = memoryAllocator->AllocateResourceToStore(renderingDevice, optionalMemoryStore, vkResourceMemoryRequirments, this);
		if (result != ALLOCATION_RESULT_SUCCESS)
		{
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanTexture::AllocAndBindVKTextureMemory(): Error allocating memory for this texture (Custom defined store)\n");
			return false;
		}
	}
	else
	{
		//Let the allocator decide on a store to use -> A public store that already exists
		//or will create a new one for us.
		SuballocationResult result = memoryAllocator->AllocateResourceAuto(renderingDevice, this);
		if (result != ALLOCATION_RESULT_SUCCESS)
		{
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanTexture::AllocAndBindVKTextureMemory(): Error allocating memory for this texture\n");
			return false;
		}
	}

	//Bind texture to device memory
	if (resourceMemoryBlock)
	{
		VkDeviceMemory memory = resourceMemoryBlock->GetParentStore()->GetVKDeviceMemoryHandle();
		VkDeviceSize offsetAligned = resourceMemoryBlock->GetBlockAlignedOffsetInStoreBytes();
		VkResult result = vkBindImageMemory(cachedVkDevice, vkImageHandle, memory, offsetAligned);
		if (result != VK_SUCCESS)
		{
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanTexture::AllocAndBindVKTextureMemory(): Error - Could not bind memory to assigned memory block\n");
			return false;
		}
	}
	else
	{
		//No memory block available - unknown error 
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanTexture::AllocAndBindVKTextureMemory() Error - No DeviceMemoryBlock assigned for this resource to bind memory with\n");
		return false;
	}

	//Done
	return true;
}

void VulkanTexture::Shutdown()
{
	if (vkImageHandle != VK_NULL_HANDLE)
	{
		vkDestroyImage(cachedVkDevice, vkImageHandle, nullptr);
		vkImageHandle = VK_NULL_HANDLE;
	}

	//Shutdown resource
	__super::Shutdown();
}