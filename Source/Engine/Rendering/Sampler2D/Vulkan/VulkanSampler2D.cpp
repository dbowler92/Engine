#include "VulkanSampler2D.h"

using namespace EngineAPI::Rendering::Platform;

void VulkanSampler2D::Shutdown()
{
	//...Shouldnt be still handing around...
	CleanupGLIData();
	CleanupLodePNGData();

	//Destroy self...
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
		//
		//TODO
		//
		//lodePNG
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
	//Allocate the texture/image
	if (!AllocAndBindVKTextureMemory(renderingDevice, optionalDeviceStore))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSampler2D::AllocAndBindVKSampler2D Error - Could not Allocate texture\n");
		return false;
	}

	//Write data
	uint8_t* data = nullptr;
	if (gliTexture2D)
		data = (uint8_t*)gliTexture2D->data();

	if (!WriteParsedTextureDataToMemory(data))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSampler2D::AllocAndBindVKSampler2D() Error: Could not write texture data to memory\n");
		return false;
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

	//Done
	return true;
}

bool VulkanSampler2D::InitVKSampler2DViews(EngineAPI::Graphics::RenderDevice* renderingDevice)
{

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
	void* mappedMemory = MapResource();
	assert(mappedMemory != nullptr);

	//Write data


	//Unmap
	UnmapResource();

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