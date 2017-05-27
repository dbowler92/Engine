#include "VulkanStatics.h"

//May need debug logging
#include "../../Debug/Log/DebugLog.h"

//Complete forward declarations
#include "../../Graphics/GraphicsPipelineCache/GraphicsPipelineCache.h"

using namespace EngineAPI::Statics;

bool VulkanStatics::CommandBufferReset(VkCommandBuffer* cmdBuffer, bool shouldReleaseMemoryToPool)
{
	VkResult result = vkResetCommandBuffer(*cmdBuffer, shouldReleaseMemoryToPool ? VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : NULL);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanStatics::VKCommandBufferReset() - Error when resetting command buffer.\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanStatics::CommandBufferBeginRecording(VkCommandBuffer* cmdBuffer, VkCommandBufferBeginInfo* cmdBufferBeginInfo)
{	
	//Begin reading to this cmd buffer (use passed in info struct) 
	VkResult result = vkBeginCommandBuffer(*cmdBuffer, cmdBufferBeginInfo);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanStatics::VKCommandBufferBeginRecording() - Error when beginning command buffer\n");
		return false;
	}
	
	//Done
	return true;
}

bool VulkanStatics::CommandBufferBeginRecordingDefault(VkCommandBuffer* cmdBuffer)
{
	//Default Inheritance info
	VkCommandBufferInheritanceInfo defaultInheritanceInfo = {};
	defaultInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	defaultInheritanceInfo.pNext = nullptr;
	defaultInheritanceInfo.renderPass = VK_NULL_HANDLE;
	defaultInheritanceInfo.subpass = 0;
	defaultInheritanceInfo.framebuffer = VK_NULL_HANDLE;
	defaultInheritanceInfo.occlusionQueryEnable = VK_FALSE;
	defaultInheritanceInfo.queryFlags = 0;
	defaultInheritanceInfo.pipelineStatistics = 0;

	//Default buffer begin reading info
	VkCommandBufferBeginInfo defaultBeginInfo = {};
	defaultBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	defaultBeginInfo.pNext = nullptr;
	defaultBeginInfo.flags = 0;
	defaultBeginInfo.pInheritanceInfo = &defaultInheritanceInfo;

	//Begin reading to this cmd buffer using default info
	VkResult result = vkBeginCommandBuffer(*cmdBuffer, &defaultBeginInfo);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanStatics::VKCommandBufferBeginRecordingDefault() - Error when beginning command buffer\n");
		return false;
	}

	//Done
	return true;
}


bool VulkanStatics::CommandBufferEndRecording(VkCommandBuffer* cmdBuffer)
{
	VkResult result = vkEndCommandBuffer(*cmdBuffer);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanStatics::VKCommandBufferEndRecording() - Error when ending recording to command buffer.\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanStatics::FindMemoryTypeForProperties(uint32_t memoryTypeBits, VkMemoryPropertyFlags properties,
	const VkPhysicalDeviceMemoryProperties* physicalDeviceMemoryProperties,
	uint32_t* memTypeIndexOut)
{
	//
	//See official docs: https://www.khronos.org/registry/vulkan/specs/1.0/man/html/VkPhysicalDeviceMemoryProperties.html
	//
	//Required:
	for (int32_t i = 0; i < physicalDeviceMemoryProperties->memoryTypeCount; ++i)
	{
		if ((memoryTypeBits & (1 << i)) &&
			((physicalDeviceMemoryProperties->memoryTypes[i].propertyFlags & properties) == properties))
		{
			*memTypeIndexOut = i;
			return true;
		}
	}

	//Failed.
	*memTypeIndexOut = 0;
	return false;
}

VkDeviceSize VulkanStatics::CalculateAlignedMemoryOffsetShiftRight(VkDeviceSize memoryOffset, VkDeviceSize memoryAlignmentRequirment)
{
	VkDeviceSize alignedOffset = memoryOffset;

	//How many bytes to shift this offset to the right?
	VkDeviceSize bytesToShiftToMakeAligned = 0;
	VkDeviceSize missAlignment = (alignedOffset % memoryAlignmentRequirment);

	if (missAlignment != 0) //Could already be aligned
		bytesToShiftToMakeAligned = memoryAlignmentRequirment - missAlignment;

	//Shift to the right
	alignedOffset += bytesToShiftToMakeAligned;

	//Done
	return alignedOffset;
}

bool VulkanStatics::MergeGraphicsPipelineCaches(VkDevice* device,
	EngineAPI::Graphics::GraphicsPipelineCache* destinationPCO,
	EngineAPI::Graphics::GraphicsPipelineCache* sourcePipelineCachesArray, uint32_t sourcePipelinesCount)
{
	//
	//TODO: Validate this
	//

	//Quick and dirty check
	assert(destinationPCO != nullptr);
	assert(sourcePipelineCachesArray != nullptr);
	assert(sourcePipelinesCount > 0);

	//Source array of PCOs
	std::vector<VkPipelineCache> srcCaches(sourcePipelinesCount);
	for (int i = 0; i < sourcePipelinesCount; i++)
		srcCaches[i] = sourcePipelineCachesArray[i].GetVKPipelineCacheHandle();

	//Do merge
	VkResult result = vkMergePipelineCaches(*device, destinationPCO->GetVKPipelineCacheHandle(),
		sourcePipelinesCount, srcCaches.data());

	//Check for error
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanStatics::MergeGraphicsPipelineCaches() Error. Could not merge pipeline caches\n");
		return false;
	}

	//Inform the destination PCO is has had other PCOs merged in to it
	destinationPCO->OnPCOMergeEvent(true);

	//Inform the source PCOs that it has been merged
	for (int i = 0; i < sourcePipelinesCount; i++)
		sourcePipelineCachesArray[i].OnPCOMergeEvent(false);
	
	//Cleanup
	srcCaches.clear();

	//Done
	return true;
}

bool VulkanStatics::CreateVKTextureView(VkDevice* device, VkImageViewCreateInfo* viewCreateInfo, VkImageView* imageViewHandleOut)
{
	VkResult result = vkCreateImageView(*device, viewCreateInfo, nullptr, imageViewHandleOut);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanStatics::CreateVKTextureView(): Could not create VkImageView object\n");
		return false;
	}

	//Done
	return true;
}

void VulkanStatics::DestoryVKTextureView(VkDevice* device, VkImageView* imageView)
{
	vkDestroyImageView(*device, *imageView, nullptr);
}

bool VulkanStatics::CreateVKBufferView(VkDevice* device, VkBufferViewCreateInfo* viewCreateInfo, VkBufferView* bufferViewHandleOut)
{
	VkResult result = vkCreateBufferView(*device, viewCreateInfo, nullptr, bufferViewHandleOut);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanStatics::CreateVKBufferView() Error: Failed to create buffer view\n");
		return false;
	}

	//Done
	return true;
}

void VulkanStatics::DestroyVKBufferView(VkDevice* device, VkBufferView* bufferView)
{
	vkDestroyBufferView(*device, *bufferView, nullptr);
}


//
//Commands
//

/*
bool VulkanCommands::VKCMD_SetImageLayout(const VkImage& image,
	VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkAccessFlagBits srcAccessMask,
	const VkCommandBuffer& commandBuffer)
{
	//Ensure a valid command buffer
	if (commandBuffer == VK_NULL_HANDLE)
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommands::VKCMD_SetImageLayout() Error - Command buffer is NULL\n");
		return false;
	}

	//Ensure valid image
	if (image == VK_NULL_HANDLE)
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommands::VKCMD_SetImageLayout() Error - Image is NULL\n");
		return false;
	}

	//Image memory barrier description
	VkImageMemoryBarrier imageMemoryBarrier = {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.pNext = nullptr;
	imageMemoryBarrier.srcAccessMask = srcAccessMask;
	imageMemoryBarrier.dstAccessMask = 0;
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imageMemoryBarrier.subresourceRange.levelCount = 1;
	imageMemoryBarrier.subresourceRange.layerCount = 1;

	if (oldImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	switch (newImageLayout)
	{
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		case  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		{
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			break;
		}

		//TODO
	}

	//Insert at top
	VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

	//Insert image memory barrier command. 
	vkCmdPipelineBarrier(commandBuffer, 
		srcStages, destStages, 0, 
		0, nullptr, 
		0, nullptr, 
		1, &imageMemoryBarrier);

	//Done
	return true;
}
*/

bool VulkanCommands::VKCMD_SetImageLayout(const VkCommandBuffer& commandBuffer, const VkImage& image,
	VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout)
{
	//
	//See: https://harrylovescode.gitbooks.io/vulkan-api/content/chap07/chap07.html
	//

	//Ensure a valid command buffer
	if (commandBuffer == VK_NULL_HANDLE)
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommands::VKCMD_SetImageLayout() Error - Command buffer is NULL\n");
		return false;
	}

	//Ensure valid image
	if (image == VK_NULL_HANDLE)
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommands::VKCMD_SetImageLayout() Error - Image is NULL\n");
		return false;
	}

	//Image memory barrier description
	VkImageMemoryBarrier imageMemoryBarrier = {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.pNext = nullptr;
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imageMemoryBarrier.subresourceRange.levelCount = 1;
	imageMemoryBarrier.subresourceRange.layerCount = 1;

	//srcAccessMask  && dstAccessMask depends on the values of the old and new image 
	//layout. 
	switch (oldImageLayout)
	{
		case VK_IMAGE_LAYOUT_PREINITIALIZED:
		{
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		{
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		{
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		{
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		{
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		}
	}

	switch (newImageLayout)
	{
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		{
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		{
			imageMemoryBarrier.srcAccessMask |= VK_ACCESS_TRANSFER_READ_BIT;
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		{
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		{
			imageMemoryBarrier.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		{
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		}
	}


	//Insert at top
	VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

	//Insert image memory barrier command. 
	vkCmdPipelineBarrier(commandBuffer,
		srcStages, destStages, 0,
		0, nullptr,
		0, nullptr,
		1, &imageMemoryBarrier);

	//Done
	return true;
}