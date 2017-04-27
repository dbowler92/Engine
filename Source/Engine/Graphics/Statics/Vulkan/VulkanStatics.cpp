#include "VulkanStatics.h"

//May need debug logging
#include "../../../Debug/Log/DebugLog.h"

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
	//TODO: Default Inheritance info...
	VkCommandBufferInheritanceInfo defaultInheritanceInfo = {};

	//Default buffer begin reading info
	VkCommandBufferBeginInfo defaultBeginInfo = {};
	defaultBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	defaultBeginInfo.pNext = nullptr;
	defaultBeginInfo.pInheritanceInfo = nullptr; //TODO. defaultInheritanceInfo
	defaultBeginInfo.flags = NULL; //TODO

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
