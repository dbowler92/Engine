#include "VulkanSampler2D.h"

using namespace EngineAPI::Rendering::Platform;

void VulkanSampler2D::Shutdown()
{
	//Destroy self...	
	if (vkSampler2DTextureImageLayoutCmdBuffer != VK_NULL_HANDLE)
		EngineAPI::Statics::VulkanStatics::CommandBufferReset(&vkSampler2DTextureImageLayoutCmdBuffer, true);

	if (vkShaderSamplerView != VK_NULL_HANDLE)
		EngineAPI::Statics::VulkanStatics::DestoryVKTextureView(&cachedVkDevice, &vkShaderSamplerView);

	//Destroy super
	__super::Shutdown();
}

bool VulkanSampler2D::InitVKSampler2D(EngineAPI::Graphics::RenderDevice* renderingDevice,
	uint32_t imageWidth, uint32_t imageHeight, uint32_t mipsCount,
	TextureTilingMode tilingMode, RenderingResourceUsage resourceUsage,
	VkFormat desiredImageFormat, VkImageUsageFlags desiredImageUsageFlags)
{
	//Fillout image creation struct
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = NULL;
	imageCreateInfo.flags = 0;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = desiredImageFormat;
	imageCreateInfo.extent.width = imageWidth;
	imageCreateInfo.extent.height = imageHeight;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = mipsCount;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.pQueueFamilyIndices = nullptr;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.usage = desiredImageUsageFlags;

	if (tilingMode == TEXTURE_TILING_MODE_LINEAR)
	{
		imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
	}
	else
	{
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		//Will be written in to by staging buffer
		if ((desiredImageUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) == 0)
			imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}

	//Init VkImage
	if (!InitVKTexture(renderingDevice, &imageCreateInfo, resourceUsage))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSampler2D::InitVKSampler2DFromTexture() Error - Could not init VkImage object\n");
		return false;
	}

	//Will we need the use of staging buffer?
	doesUseStagingBuffer = false;
	if (tilingMode == TEXTURE_TILING_MODE_OPTIMAL)
		doesUseStagingBuffer = true;
	if (resourceUsage != RENDERING_RESOURCE_USAGE_GPU_READ_CPU_WRITE)
		doesUseStagingBuffer = true;

	//Done
	return true;
}

/*
bool VulkanSampler2D::InitVKSampler2DFromTexture(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Rendering::TextureData* textureData, TextureTilingMode tilingMode, RenderingResourceUsage resourceUsage,
	VkFormat desiredImageFormat, VkImageUsageFlags desiredImageUsageFlags)
{
	//TEMP - Linear + dynamic texture only for now... For texture data stored in 
	//device local memory, use InitVKSampler2DFromStagingBuffer() instead!
	assert(tilingMode == TEXTURE_TILING_MODE_LINEAR);
	assert(resourceUsage == RENDERING_RESOURCE_USAGE_GPU_READ_CPU_WRITE);

	//Error checks
	assert(textureData != nullptr);
	if (tilingMode == TEXTURE_TILING_MODE_OPTIMAL)
	{
		if (resourceUsage == RENDERING_RESOURCE_USAGE_GPU_READ_CPU_WRITE)
		{
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSampler2D::InitVKSampler2DFromTexture() Error: Texture can not have a Dynamic usage if using optimal tiling mode\n");
			return false;
		}
	}

	//TODO: Auto Generate mips????

	//Fillout image creation struct
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = NULL;
	imageCreateInfo.flags = 0;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = desiredImageFormat;
	imageCreateInfo.extent.width = textureData->GetTextureWidth();
	imageCreateInfo.extent.height = textureData->GetTextureHeight();
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = textureData->GetMipmapLevelsCount();
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.pQueueFamilyIndices = nullptr;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.usage = desiredImageUsageFlags;

	if (tilingMode == TEXTURE_TILING_MODE_LINEAR)
	{
		imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
	}
	else
	{
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		//Will be written in to by staging buffer
		if ((desiredImageUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) == 0)
			imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}

	//Init VkImage
	if (!InitVKTexture(renderingDevice, &imageCreateInfo, resourceUsage))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSampler2D::InitVKSampler2DFromTexture() Error - Could not init VkImage object\n");
		return false;
	}

	//Init the staging buffer if required.
	doesUseStagingBuffer = false;
	if (tilingMode == TEXTURE_TILING_MODE_OPTIMAL)  //TODO: Or usage == ...
		doesUseStagingBuffer = true;

	if (doesUseStagingBuffer)
	{
		if (this->GetResourceDebugName().empty() == true)
			stagingBuffer.SetResourceDebugName("Resource Staging Buffer");
		else
			stagingBuffer.SetResourceDebugName(this->GetResourceDebugName() + " Staging Buffer");

		assert(stagingBuffer.InitVKStagingBuffer(renderingDevice, textureData->GetTextureDataSize()));
	}

	//Done
	return true;
}

bool VulkanSampler2D::InitVKSampler2DFromStagingBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Graphics::StagingBuffer* stagingBuffer, uint32_t imageWidth, uint32_t imageHeight, uint32_t mipsCount,
	TextureTilingMode tilingMode, RenderingResourceUsage resourceUsage,
	VkFormat desiredImageFormat, VkImageUsageFlags desiredImageUsageFlags)
{
	//TEMP: Can't be dynamic texture
	assert(resourceUsage != RENDERING_RESOURCE_USAGE_GPU_READ_CPU_WRITE);

	//Error check
	assert(stagingBuffer != nullptr);

	//TODO: Auto Generate mips????

	//Fillout image creation struct
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = NULL;
	imageCreateInfo.flags = 0;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = desiredImageFormat;
	imageCreateInfo.extent.width = imageWidth;
	imageCreateInfo.extent.height = imageHeight;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = mipsCount;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.pQueueFamilyIndices = nullptr;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.usage = desiredImageUsageFlags;

	if (tilingMode == TEXTURE_TILING_MODE_LINEAR)
	{
		imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
	}
	else
	{
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		//Will be written in to by staging buffer
		if ((desiredImageUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) == 0)
			imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}

	//Init VkImage
	if (!InitVKTexture(renderingDevice, &imageCreateInfo, resourceUsage))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSampler2D::InitVKSampler2DFromTexture() Error - Could not init VkImage object\n");
		return false;
	}

	//Does use staging buffer
	doesUseStagingBuffer = true;

	//Done
	return true;
}
*/

bool VulkanSampler2D::AllocAndBindVKSampler2D(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Graphics::DeviceMemoryStore* optionalDeviceStore)
{
	//Allocate the texture/image block
	if (!AllocAndBindVKTextureMemory(renderingDevice, optionalDeviceStore))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSampler2D::AllocAndBindVKSampler2D Error - Could not Allocate texture\n");
		return false;
	}
	
	//Done
	return true;
}
 
bool VulkanSampler2D::WriteTextureDataFromTexture(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Rendering::TextureData* textureData)
{
	assert(textureData != nullptr);

	//Ensure valid call
	assert(!doesUseStagingBuffer);

	//Write data to the resource block
	assert(WriteParsedTextureDataToMemory(textureData->GetRawTextureData()));

	//Done
	return true;
}

bool VulkanSampler2D::WriteTextureDataFromStagingBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Graphics::StagingBuffer* stagingBuffer)
{
	assert(stagingBuffer != nullptr);

	//Ensure valid call
	assert(doesUseStagingBuffer);

	//Copy staging buffer data in to the resource block. 

	//Done
	return true;
}

bool VulkanSampler2D::InitVKSampler2DLayoutAndViews(EngineAPI::Graphics::RenderDevice* renderingDevice)
{
	//Layout
	if (!InitVKSampler2DLayout(renderingDevice))
		return false;

	//Views
	if (!InitVKSampler2DViews(renderingDevice))
		return false;

	//Done
	return true;
}

bool VulkanSampler2D::InitVKSampler2DLayout(EngineAPI::Graphics::RenderDevice* renderingDevice)
{
	//Get a command buffer from the graphics command buffer pool (we will submit the command buffer
	//to the graphics queue later)
	EngineAPI::Graphics::CommandBufferPool& cmdPool = renderingDevice->GetGraphicsCommandQueueFamily()->GetCommandBufferPool(0);
	if (!cmdPool.GetVKCommandBufferFromPool(true, &vkSampler2DTextureImageLayoutCmdBuffer))
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSampler2D::InitVKSampler2DLayout() - Could not get command buffer from pool\n");
		return false;
	}

	//Begin reading commands to this buffer
	if (EngineAPI::Statics::VulkanStatics::CommandBufferBeginRecordingDefault(&vkSampler2DTextureImageLayoutCmdBuffer))
	{
		VkImageSubresourceRange subresourceRange = {};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.levelCount = mipLevelsCount;
		subresourceRange.layerCount = 1;

		VkImageLayout oldLayout;
		VkImageLayout newLayout;

		if (!doesUseStagingBuffer)
		{
			oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
			newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		}
		else
		{
			oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		}

		EngineAPI::Statics::VulkanCommands::CMD_SetImageLayout(vkSampler2DTextureImageLayoutCmdBuffer,
			vkImageHandle, subresourceRange.aspectMask, oldLayout, newLayout, subresourceRange);
	}
	else
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSampler2D::InitVKSampler2DLayout() - Failed to begin reading to command buffer\n");
		EngineAPI::Statics::VulkanStatics::CommandBufferEndRecording(&vkSampler2DTextureImageLayoutCmdBuffer);
		return false;
	}

	//Stop reading commands in to the command buffer
	EngineAPI::Statics::VulkanStatics::CommandBufferEndRecording(&vkSampler2DTextureImageLayoutCmdBuffer);

	//Submit the work -> Inits this images & its layout. 
	EngineAPI::Graphics::CommandQueueFamily* graphicsQueueFamily = renderingDevice->GetGraphicsCommandQueueFamily();

	//Ensure that the GPU has finished the submitted work before the host 
	//takes over again
	VkFence textureImageLayoutFence;
	VkDevice device = renderingDevice->GetVKLogicalDevice();
	assert(EngineAPI::Statics::VulkanStatics::CreateVKFence(&device, false, &textureImageLayoutFence));

	if (!graphicsQueueFamily->SubmitVKCommandBuffersToQueueDefault(0, &vkSampler2DTextureImageLayoutCmdBuffer, 1, textureImageLayoutFence, true))
	{
		//Error in submission
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSampler2D::InitVKSampler2DLayout() Error - Failed to submit layout command buffer to queue\n");
		return false;
	}

	//Wait on fence
	vkWaitForFences(device, 1, &textureImageLayoutFence, VK_TRUE, 10000000000);
	vkDestroyFence(device, textureImageLayoutFence, nullptr);

	//Done
	return true;
}

bool VulkanSampler2D::InitVKSampler2DViews(EngineAPI::Graphics::RenderDevice* renderingDevice)
{
	VkImageSubresourceRange subresourceRange;
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.baseArrayLayer = 0;
	subresourceRange.levelCount = 1; 
	subresourceRange.layerCount = 1;

	//Image view to allow shader access to the texture info
	VkImageViewCreateInfo viewCreateInfo = {};
	viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCreateInfo.pNext = nullptr;
	viewCreateInfo.flags = 0;
	viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewCreateInfo.format = vkTextureFormat;
	viewCreateInfo.image = vkImageHandle;
	viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
	viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
	viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
	viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
	viewCreateInfo.subresourceRange = subresourceRange;

	VkDevice device = renderingDevice->GetVKLogicalDevice();
	assert(EngineAPI::Statics::VulkanStatics::CreateVKTextureView(&device, &viewCreateInfo, &vkShaderSamplerView));

	//Done
	return true;
}

bool VulkanSampler2D::WriteParsedTextureDataToMemory(uint8_t* data)
{
	assert(data != nullptr);

	//Gets the row pitch -> Helps us properly update the texture resource
	VkImageSubresource subresource = {};
	subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresource.mipLevel = 0;
	subresource.arrayLayer = 0;
	VkSubresourceLayout layout;
	vkGetImageSubresourceLayout(cachedVkDevice, vkImageHandle, &subresource, &layout);

	//Map memory
	uint8_t* mappedMemory = (uint8_t*)MapResource();
	assert(mappedMemory != nullptr);

	//Write data
	for (int y = 0; y < vkTextureDimentions.height; y++)
	{
		size_t imageWidthSize = vkTextureDimentions.height * 4;
		memcpy(mappedMemory, data, imageWidthSize);
		
		data += imageWidthSize;
		mappedMemory += layout.rowPitch;
	}

	//Unmap
	UnmapResource();

	//Done
	return true;
}

bool VulkanSampler2D::AutoGenerateMips(uint8_t* textureData)
{
	//TODO

	//Done
	return true;
}