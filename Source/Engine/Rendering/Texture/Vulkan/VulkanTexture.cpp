#include "VulkanTexture.h"

using namespace EngineAPI::Rendering::Platform;

VulkanTexture::VulkanTexture()
	: Resource(VULKAN_RESOURCE_TYPE_TEXTURE) //Init base. 
{}

bool VulkanTexture::InitVKTexture(EngineAPI::Graphics::RenderDevice* renderingDevice, VkImageCreateInfo* imageCreateInfo)
{
	//Cache texture info for future use
	vkImageTilingMode   = imageCreateInfo->tiling;
	vkTextureDimentions = imageCreateInfo->extent;
	vkTextureFormat = imageCreateInfo->format;
	vkImageType = imageCreateInfo->imageType;
	vkImageUsageFlags = imageCreateInfo->usage;
	vkSamplesCountFlag = imageCreateInfo->samples;
	//ETC

	//Get the logical device used to create this texture. 
	cachedVkDevice = renderingDevice->GetVKLogicalDevice();

	//Create logical image with passed in data
	VkResult result = vkCreateImage(cachedVkDevice, imageCreateInfo, nullptr, &vkImageHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanTextur::InitVKTexture() - Error: Could not create VkImage\n");
		return false;
	}

	//Get memory requirements for this image
	VkMemoryRequirements textureMemoryRequirments = {};
	vkGetImageMemoryRequirements(cachedVkDevice, vkImageHandle, &textureMemoryRequirments);

	//Alloc memory for the texture on the device
	EngineAPI::Graphics::DeviceMemoryAllocator* memoryAllocator = renderingDevice->GetDeviceMemoryAllocator();
	//if (!memoryAllocator->AllocateResource(this))
	//{
	//	return false;
	//}

	//Bind texture to device memory

	//Create views


	//Done
	return true;
}

void VulkanTexture::Shutdown()
{
	vkDestroyImage(cachedVkDevice, vkImageHandle, nullptr);
	vkImageHandle = VK_NULL_HANDLE;
}