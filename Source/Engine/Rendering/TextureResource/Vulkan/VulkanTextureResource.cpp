#include "VulkanTextureResource.h"

using namespace EngineAPI::Rendering::Platform;

void VulkanTextureResource::Shutdown()
{
	vkDestroyImage(cachedVkDevice, vkImageHandle, nullptr);
	vkImageHandle = VK_NULL_HANDLE;
}

bool VulkanTextureResource::InitVKTexture(VkDevice* logicalDevice, 
	VkImageCreateInfo* imageCreateInfo)
{
	//Cache info
	cachedVkDevice = *logicalDevice;

	//Create image with passed in data
	VkResult result = vkCreateImage(cachedVkDevice, imageCreateInfo, nullptr, &vkImageHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanTextureResource::InitVKTexture() - Error: Could not create VkImage\n");
		return false;
	}

	//Done
	return true;
}