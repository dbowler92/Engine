#include "VulkanTexture2D.h"

using namespace EngineAPI::Rendering::Platform;

bool VulkanTexture2D::InitVKTexture2D(EngineAPI::Graphics::RenderDevice* renderingDevice, VkImageCreateInfo* imageCreateInfo)
{
	//Get the logical device used to create this texture. 
	cachedVkDevice = renderingDevice->GetVKLogicalDevice();

	//Create logical image with passed in data
	VkResult result = vkCreateImage(cachedVkDevice, imageCreateInfo, nullptr, &vkImageHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanTexture2D::InitVKTexture2D() - Error: Could not create VkImage\n");
		return false;
	}

	//Get memory requirements for this image
	VkMemoryRequirements textureMemoryRequirments = {};
	vkGetImageMemoryRequirements(cachedVkDevice, vkImageHandle, &textureMemoryRequirments);

	//Alloc memory for the texture on the device
	EngineAPI::Graphics::DeviceMemoryAllocator* memoryAllocator = renderingDevice->GetDeviceMemoryAllocator();

	//Bind texture to device memory

	//Create views



	//Done
	return true;
}

void VulkanTexture2D::Shutdown()
{
	vkDestroyImage(cachedVkDevice, vkImageHandle, nullptr);
	vkImageHandle = VK_NULL_HANDLE;
}