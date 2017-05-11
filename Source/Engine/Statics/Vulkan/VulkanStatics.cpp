#include "VulkanStatics.h"

//May need debug logging
#include "../../Debug/Log/DebugLog.h"

using namespace EngineAPI::Graphics;

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

bool VulkanStatics::CreateVKTextureView(VkDevice* device, VkImageViewCreateInfo* viewCreateInfo, VkImageView* imageViewHandleOut)
{
	VkResult result = vkCreateImageView(*device, viewCreateInfo, nullptr, imageViewHandleOut);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanTexture::CreateVKTextureView(): Could not create VkImageView object\n");
		return false;
	}

	//Done
	return true;
}