#include "VulkanSampler2D.h"

using namespace EngineAPI::Rendering::Platform;

void VulkanSampler2D::Shutdown()
{
	//...Shouldn't be still handing around...
	CleanupGLIData();
	CleanupLodePNGData();

	//Destroy self...
	if (vkSampler2DTextureImageLayoutCmdBuffer != VK_NULL_HANDLE)
		EngineAPI::Statics::VulkanStatics::CommandBufferReset(&vkSampler2DTextureImageLayoutCmdBuffer, true);

	if (vkShaderSamplerView != VK_NULL_HANDLE)
		EngineAPI::Statics::VulkanStatics::DestoryVKTextureView(&cachedVkDevice, &vkShaderSamplerView);

	//Destroy super
	__super::Shutdown();
}

bool VulkanSampler2D::InitVKSampler2DFromFile(EngineAPI::Graphics::RenderDevice* renderingDevice, 
	const char* filename, TextureLoadingAPI textureLoadingAPI, TextureTilingMode tilingMode,
	bool isDynamicTexture, VkFormat desiredImageFormat, VkImageUsageFlags desiredImageUsageFlags)
{
	//Temp
	assert(textureLoadingAPI == TEXTURE_LOADING_API_GLI);

	//Print message telling us what we are loading...
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanSampler2D::InitVKSampler2DFromFile(): Loading texture: ");
	EngineAPI::Debug::DebugLog::PrintMessage(filename);
	EngineAPI::Debug::DebugLog::PrintMessage("\n");

	//Load texture data
	void* rawData = nullptr;
	uint32_t imageWidth = 0;
	uint32_t imageHeight = 0;
	uint32_t mipmapLevelsCount = 0;
	if (textureLoadingAPI == TEXTURE_LOADING_API_GLI)
	{
		//GLI
		//
		//Load the file
		gliTexture2D = GE_NEW gli::texture2D(gli::load(filename));
		assert(!gliTexture2D->empty());

		//Get image dimensions at the top sub-resource level
		imageWidth = (uint32_t)(*gliTexture2D)[0].dimensions().x;
		imageHeight = (uint32_t)(*gliTexture2D)[0].dimensions().y;

		//Mip levels in the parsed image
		mipmapLevelsCount = gliTexture2D->levels();

		//Raw image data
		rawData = gliTexture2D->data();
	}
	if (textureLoadingAPI == TEXTURE_LOADING_API_LODE_PNG)
	{
		//lodePNG
		std::vector<unsigned char>textureBuffer;
		lodepng::load_file(textureBuffer, filename);
	}

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
	imageCreateInfo.mipLevels = mipmapLevelsCount;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.pQueueFamilyIndices = nullptr;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.usage = desiredImageUsageFlags;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
	if (tilingMode == TEXTURE_TILING_MODE_LINEAR)
		imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
	else
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL; //TODO

	//Init VkImage
	if (!InitVKTexture(renderingDevice, &imageCreateInfo, isDynamicTexture))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSampler2D::InitVKSampler2DFromFile() Error - Could not init VkImage object\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanSampler2D::AllocAndBindVKSampler2D(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Graphics::DeviceMemoryStore* optionalDeviceStore)
{
	//Allocate the texture/image block
	if (!AllocAndBindVKTextureMemory(renderingDevice, optionalDeviceStore))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSampler2D::AllocAndBindVKSampler2D Error - Could not Allocate texture\n");
		return false;
	}

	//Write data - If linear, just copy the data across to the resource
	if (vkImageTilingMode == VK_IMAGE_TILING_LINEAR)
	{
		uint8_t* data = nullptr;
		if (gliTexture2D)
			data = (uint8_t*)gliTexture2D->data();

		if (!WriteParsedTextureDataToMemory(data))
		{
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSampler2D::AllocAndBindVKSampler2D() Error: Could not write texture data to memory\n");
			return false;
		}
	}
	else if (vkImageTilingMode == VK_IMAGE_TILING_LINEAR)
	{
		//Use staging buffer to copy data to the GPU

	}
	
	//Cleanup CPU copy of texture
	CleanupGLIData();
	CleanupLodePNGData();

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

		VkImageLayout oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
		VkImageLayout newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

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

void VulkanSampler2D::CleanupGLIData()
{
	if (gliTexture2D)
	{
		delete gliTexture2D;
		gliTexture2D = nullptr;
	}
}

void VulkanSampler2D::CleanupLodePNGData()
{
	//TODO
}