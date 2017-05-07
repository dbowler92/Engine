#include "VulkanDepthTexture.h"

using namespace EngineAPI::Rendering::Platform;

bool VulkanDepthTexture::InitVKDepthTexture(EngineAPI::Graphics::RenderDevice* renderingDevice,
	DepthTextureFormat depthTextureFormat, ESize2D depthTextureDimentions,
	DepthTextureUsageFlag depthTextureUsageFlags)
{
	//Cache state
	this->depthTextureFormat = depthTextureFormat;
	this->depthTextureUsage = depthTextureUsageFlags;

	//VK format for the depth texture based on the platform agnostic input
	VkFormat depthTextureFormatVK;
	if (depthTextureFormat == DEPTH_TEXTURE_FORMAT_D16)
		depthTextureFormatVK = VK_FORMAT_D16_UNORM;
	//if (depthTextureFormat == DEPTH_TEXTURE_FORMAT_D24)
	//	depthTextureFormatVK = VK_FORMAT_D24_UNORM_S8_UINT;
	if (depthTextureFormat == DEPTH_TEXTURE_FORMAT_D32)
		depthTextureFormatVK = VK_FORMAT_D32_SFLOAT;
	if (depthTextureFormat == DEPTH_TEXTURE_FORMAT_D24_S8)
		depthTextureFormatVK = VK_FORMAT_D24_UNORM_S8_UINT;

	//Usage of the VKImage based on desired input. 
	VkImageUsageFlags depthTextureVKImageUsageFlag = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	if (depthTextureUsageFlags & DEPTH_TEXTURE_USAGE_SHADER_INPUT_BIT)
		depthTextureVKImageUsageFlag |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT; //VK_IMAGE_USAGE_SAMPLED_BIT ???

	//Depth buffer tiling mode - this is somewhat dependent of the depth format...
	VkFormatProperties formatProps = {};
	vkGetPhysicalDeviceFormatProperties(renderingDevice->GetVKPhysicalDevice(), depthTextureFormatVK, &formatProps);

	VkImageTiling vkImageTilingMode;
	if (formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		vkImageTilingMode = VK_IMAGE_TILING_LINEAR;
	else if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		vkImageTilingMode = VK_IMAGE_TILING_OPTIMAL;
	else
	{
		//Error...
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDepthTexture::Init() Error - Failed to find tiling mode for this texture suggesting an unsupported depth texture format\n");
		return false;
	}

	//Creation info struct
	VkImageCreateInfo depthTextureCreateInfo = {};
	depthTextureCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	depthTextureCreateInfo.pNext = nullptr;
	depthTextureCreateInfo.flags = 0;
	depthTextureCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	depthTextureCreateInfo.format = depthTextureFormatVK;
	depthTextureCreateInfo.extent.width = (uint32_t)depthTextureDimentions.Width;
	depthTextureCreateInfo.extent.height = (uint32_t)depthTextureDimentions.Height;
	depthTextureCreateInfo.extent.depth = 1;
	depthTextureCreateInfo.mipLevels = 1;
	depthTextureCreateInfo.arrayLayers = 1;
#if GRAPHICS_CONFIG_ENABLE_MSAA
	//TODO
	//depthTextureCreateInfo.samples = VK_SAMEPLE_COUNT_;
#else
	depthTextureCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
#endif
	depthTextureCreateInfo.pQueueFamilyIndices = nullptr;
	depthTextureCreateInfo.queueFamilyIndexCount = 0;
	depthTextureCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	depthTextureCreateInfo.usage = depthTextureVKImageUsageFlag;
	depthTextureCreateInfo.tiling = vkImageTilingMode;

	//Init texture
	if (!depthTextureObject.InitVKTexture(renderingDevice, &depthTextureCreateInfo))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDepthTexture::Init() Error - Could not init Texture2D object\n");
		return false;
	}

	//Done
	return true;
}

void VulkanDepthTexture::Shutdown()
{
	//Destroy texture
	depthTextureObject.Shutdown();
}