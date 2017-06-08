#include "VulkanSampler2D.h"

//Texture loading
#include <gli/gli.hpp> //NOTE: Preprocessor define required: _CRT_SECURE_NO_WARNINGS
#include "../../3rdParty/LodePNG/lodePNG.h"

using namespace EngineAPI::Rendering::Platform;

void VulkanSampler2D::Shutdown()
{
	//Destroy self...
	if (vkSamplerView != VK_NULL_HANDLE)
		EngineAPI::Statics::VulkanStatics::DestoryVKTextureView(&cachedVkDevice, &vkSamplerView);

		//Destroy super
	__super::Shutdown();
}

bool VulkanSampler2D::InitVKSampler2DFromFile(EngineAPI::Graphics::RenderDevice* renderingDevice, 
	const char* filename, TextureLoadingAPI textureLoadingAPI, TextureTilingMode tilingMode,
	bool isDynamicTexture, VkFormat desiredImageFormat, VkImageUsageFlags desiredImageUsageFlags)
{
	//Print message telling us what we are loading...
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanSampler2D::InitVKSampler2DFromFile(): Loading texture: ");
	EngineAPI::Debug::DebugLog::PrintMessage(filename);
	EngineAPI::Debug::DebugLog::PrintMessage("\n");

	//Temp
	assert(textureLoadingAPI == TEXTURE_LOADING_API_GLI);

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
		gli::texture2D tex2D(gli::load(filename));
		assert(!tex2D.empty());

		//Get image dimensions at the top sub-resource level
		imageWidth = (uint32_t)tex2D[0].dimensions().x;
		imageHeight = (uint32_t)tex2D[0].dimensions().y;

		//Mip levels in the parsed image
		mipmapLevelsCount = tex2D.levels();

		//Raw image data
		rawData = tex2D.data();
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


	//Done
	return true;
}
 
bool VulkanSampler2D::InitVKSampler2DLayoutAndViews(EngineAPI::Graphics::RenderDevice* renderingDevice)
{


	//Done
	return true;
}