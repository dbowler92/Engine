#include "VulkanDepthTexture.h"

//Statics 
#include "../../../Statics/Vulkan/VulkanStatics.h"

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
	if (depthTextureFormat == DEPTH_TEXTURE_FORMAT_D32)
		depthTextureFormatVK = VK_FORMAT_D32_SFLOAT;

	if (depthTextureFormat == DEPTH_TEXTURE_FORMAT_D16_S8)
		depthTextureFormatVK = VK_FORMAT_D16_UNORM_S8_UINT;
	if (depthTextureFormat == DEPTH_TEXTURE_FORMAT_D24_S8)
		depthTextureFormatVK = VK_FORMAT_D24_UNORM_S8_UINT;
	if (depthTextureFormat == DEPTH_TEXTURE_FORMAT_D32_S8)
		depthTextureFormatVK = VK_FORMAT_D32_SFLOAT_S8_UINT;

	//Does this depth texture contain a stencil component?
	doesContainStencilComponent = false;
	if (depthTextureFormat == DEPTH_TEXTURE_FORMAT_D16_S8 || depthTextureFormat == DEPTH_TEXTURE_FORMAT_D24_S8 || depthTextureFormat == DEPTH_TEXTURE_FORMAT_D32_S8)
		doesContainStencilComponent = true;

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
	//depthTextureCreateInfo.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL

	//Init texture/image (parent)
	if (!InitVKTexture(renderingDevice, &depthTextureCreateInfo, false))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDepthTexture::Init() Error - Could not init VkImage object\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanDepthTexture::AllocAndBindVKDepthTexture(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Graphics::DeviceMemoryStore* optionalDeviceStore)
{
	//Allocate the texture/image
	if (!AllocAndBindVKTextureMemory(renderingDevice, optionalDeviceStore))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDepthTexture::Init() Error - Could not Allocate texture\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanDepthTexture::InitVKDepthTextureLayout(EngineAPI::Graphics::RenderDevice* renderingDevice)
{
	//TODO

	//Done
	return true;
}

bool VulkanDepthTexture::InitVKDepthTextureViews(EngineAPI::Graphics::RenderDevice* renderingDevice)
{
	VkImageViewCreateInfo imageViewCreateInfo = {};

	//Should we create a depth stencil view?
	if (vkImageUsageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
	{
		//Depth(stencil) view
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = nullptr;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = vkImageHandle;
		imageViewCreateInfo.format = vkTextureFormat;
		imageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY };
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (doesContainStencilComponent)
			imageViewCreateInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		if (!EngineAPI::Statics::VulkanStatics::CreateVKTextureView(&cachedVkDevice, &imageViewCreateInfo, &vkDepthStencilView))
		{
			//Error
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDepthTexture::InitVKDepthTextureViews() - Error creating depth/stencil image view\n");
			return false;
		}
	}

	//Should we create a shader resource view?
	if (vkImageUsageFlags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT) //VERIFY
	{
		//Reset image view struct
		imageViewCreateInfo = {};

		//TODO

		//TEMP
		return false;
	}

	//Done
	return true;
}

void VulkanDepthTexture::Shutdown()
{
	//Destroy image views
	EngineAPI::Statics::VulkanStatics::DestoryVKTextureView(&cachedVkDevice, &vkDepthStencilView);

	//Destroy super
	__super::Shutdown();
}