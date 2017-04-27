#include "VulkanCommandQueue.h"

//Debug
#include "../../../Debug/Log/DebugLog.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanCommandQueue::Shutdown()
{
	//?
}

bool VulkanCommandQueue::InitVKQueue(VkDevice* logicalDevice, uint32_t queueFamilyIndex, uint32_t queueIndex)
{
	//Cache VkQueue handle
	vkGetDeviceQueue(*logicalDevice, queueFamilyIndex, queueIndex, &vkQueueHandle);

	//Cache device handle for future use (eg: Creating fences)
	cachedVkDeviceHandle = *logicalDevice;

	//Done
	return true;
}

bool VulkanCommandQueue::SubmitVKCommandBuffer(VkCommandBuffer* cmdBuffer)
{
	//Submission info
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = cmdBuffer;

	//Submit - don't wait
	VkResult result = vkQueueSubmit(vkQueueHandle, 1, &submitInfo, NULL);
	if (result != VK_SUCCESS)
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommandQueue: Error when submititng VkCommandBuffer\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanCommandQueue::SubmitVKCommandBufferAndWait(VkCommandBuffer* cmdBuffer)
{
	//TODO

	VkFence fence; //Signalled when cmd buffer has finished executing
	//vkCreateFence();

	//Done
	return true;
}

bool VulkanCommandQueue::SubmitVKCommandBuffersArray(VkCommandBuffer* cmdBuffersArray, uint32_t cmdBuffersCount)
{
	//TODO

	//Done
	return true;
}

bool VulkanCommandQueue::SubmitVKCommandBuffersArrayAndWait(VkCommandBuffer* cmdBuffersArray, uint32_t cmdBuffersCount)
{
	//TODO

	//VkFence fence; //Signalled when cmd buffer has finished executing
	//vkCreateFence();

	//Done
	return true;
}