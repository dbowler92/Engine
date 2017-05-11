#include "VulkanTexture.h"

//Memory allocator
#include "../../../Graphics/DeviceMemoryAllocator/DeviceMemoryAllocator.h"

using namespace EngineAPI::Rendering::Platform;

VulkanTexture::VulkanTexture()
	: Resource(RENDERING_RESOURCR_TYPE_DEPTH_TEXTURE) //Init base. 
{}

bool VulkanTexture::InitVKTexture(EngineAPI::Graphics::RenderDevice* renderingDevice, 
	VkImageCreateInfo* imageCreateInfo, bool isTextureDynamic,
	EngineAPI::Graphics::DeviceMemoryStore* optionalMemoryStore)
{
	//Cache texture info for future use
	vkImageTilingMode   = imageCreateInfo->tiling;
	vkTextureDimentions = imageCreateInfo->extent;
	vkTextureFormat = imageCreateInfo->format;
	vkImageType = imageCreateInfo->imageType;
	vkImageUsageFlags = imageCreateInfo->usage;
	vkSamplesCountFlag = imageCreateInfo->samples;
	//ETC

	//Do we want a dynamic texture
	isVkTextureDynamic = isTextureDynamic;

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
	vkGetImageMemoryRequirements(cachedVkDevice, vkImageHandle, &vkTextureMemoryRequirments);

	//Alloc memory for the texture on the device
	EngineAPI::Graphics::DeviceMemoryAllocator* memoryAllocator = renderingDevice->GetDeviceMemoryAllocator();
	if (optionalMemoryStore)
	{
		//Use passed in memory store
		SuballocationResult result = memoryAllocator->AllocateResourceToStore(renderingDevice, optionalMemoryStore, vkTextureMemoryRequirments, this);
		if (result != ALLOCATION_RESULT_SUCCESS)
		{
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanTexture::InitVKTexture(): Error allocating memory for this texture (Custom defined store)\n");
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
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanTexture::InitVKTexture(): Error allocating memory for this texture\n");
			return false;
		}
	}

	//Bind texture to device memory
	if (resourceMemoryBlock)
	{
		EngineAPI::Graphics::DeviceMemoryStore* s0 = resourceMemoryBlock->GetParentStore();

		VkDeviceMemory memory = resourceMemoryBlock->GetParentStore()->GetVKDeviceMemoryHandle();
		VkDeviceSize offsetAligned = resourceMemoryBlock->GetBlockAlignedOffsetInStoreBytes();
		VkResult result = vkBindImageMemory(cachedVkDevice, vkImageHandle, memory, offsetAligned);
		//if (result != VK_SUCCESS)
		//{
		//	EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanTexture::InitVKTexture(): Error - Could not bind memory to assigned memory block\n");
		//	return false;
		//}

		EngineAPI::Graphics::DeviceMemoryStore* s = resourceMemoryBlock->GetParentStore();
		EngineAPI::Graphics::DeviceMemoryAllocator* a = renderingDevice->GetDeviceMemoryAllocator();
		a->Debug_LongDump("VulkanMemoryDump");
		//s->GetParentAllocator()->Debug_LongDump("MemoryLog_1");

	}
	else
	{
		//No memory block available - unknown error 
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanTexture::InitVKTexture() Error - No DeviceMemoryBlock assigned for this resource to bind memory with\n");
		return false;
	}

	EngineAPI::Graphics::DeviceMemoryStore* s = resourceMemoryBlock->GetParentStore();
	//s->GetParentAllocator()->Debug_LongDump("MemoryLog_1");

	//Done
	return true;
}

bool VulkanTexture::CreateVKTextureView(VkImageViewCreateInfo* viewCreateInfo, VkImageView* imageViewHandleOut)
{
	VkResult result = vkCreateImageView(cachedVkDevice, viewCreateInfo, nullptr, imageViewHandleOut);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanTexture::CreateVKTextureView(): Could not create VkImageView object\n");
		return false;
	}

	//Done
	return true;
}

void VulkanTexture::Shutdown()
{
	vkDestroyImage(cachedVkDevice, vkImageHandle, nullptr);
	vkImageHandle = VK_NULL_HANDLE;
}