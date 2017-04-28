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

bool VulkanCommandQueue::BlockUntilVKQueueIdle()
{
	VkResult result = vkQueueWaitIdle(vkQueueHandle);
	if (result != VK_SUCCESS)
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommandQueue::BlockUntilVKQueueIdle() - Error when waiting for queue to become idle\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanCommandQueue::SubmitVKCommandBuffers(VkSubmitInfo* submitInfos, uint32_t submitInfosCount,
	const VkFence& optionalFence, bool doWaitOnQueueIdle)
{
	//Use passed in data
	VkResult result = vkQueueSubmit(vkQueueHandle, submitInfosCount, submitInfos, optionalFence);
	if (result != VK_SUCCESS)
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommandQueue::SubmitVKCommandBuffers() - Error when submititng VkCommandBuffers\n");
		return false;
	}

	//Wait on queue idle?
	if (doWaitOnQueueIdle)
		return BlockUntilVKQueueIdle();

	//Done
	return true;
}

bool VulkanCommandQueue::SubmitVKCommandBuffersDefault(VkCommandBuffer* cmdBuffers, uint32_t cmdBufferCount,
	const VkFence& optionalFence, bool doWaitOnQueueIdle)
{
	//Generate a generic / default VkSubmitInfos struct
	VkSubmitInfo defaultSubmitInfo = {};
	defaultSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	defaultSubmitInfo.pNext = nullptr;
	defaultSubmitInfo.waitSemaphoreCount = 0;
	defaultSubmitInfo.pWaitSemaphores = nullptr;
	defaultSubmitInfo.signalSemaphoreCount = 0;
	defaultSubmitInfo.pSignalSemaphores = nullptr;
	defaultSubmitInfo.pWaitDstStageMask = nullptr;
	defaultSubmitInfo.commandBufferCount = cmdBufferCount;
	defaultSubmitInfo.pCommandBuffers = cmdBuffers;

	//Submit
	VkResult result = vkQueueSubmit(vkQueueHandle, 1, &defaultSubmitInfo, optionalFence);
	if (result != VK_SUCCESS)
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommandQueue::SubmitVKCommandBuffersDefault() - Error when submititng VkCommandBuffers\n");
		return false;
	}

	//Wait on queue to become idle again?
	if (doWaitOnQueueIdle)
		return BlockUntilVKQueueIdle();
	
	//Done
	return true;
}