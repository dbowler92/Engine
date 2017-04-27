#include "VulkanCommandQueue.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanCommandQueue::Shutdown()
{
	//?
}

bool VulkanCommandQueue::InitVKQueue(VkDevice* logicalDevice, uint32_t queueFamilyIndex, uint32_t queueIndex)
{
	//Cache VkQueue handle
	vkGetDeviceQueue(*logicalDevice, queueFamilyIndex, queueIndex, &vkQueueHandle);

	//Done
	return true;
}

bool VulkanCommandQueue::SubmitCommandBuffer(EngineAPI::Graphics::RenderCommandBuffer* cmdBuffer)
{
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	//submitInfo.

	VkResult result = vkQueueSubmit(vkQueueHandle, 1, &submitInfo, NULL);
	if (result != VK_SUCCESS)
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommandQueue: Error when submititng buffer\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanCommandQueue::SubmitCommandBufferAndWait(EngineAPI::Graphics::RenderCommandBuffer* cmdBuffer)
{
	//TODO

	VkFence fence; //Signalled when cmd buffer has finished executing
	//vkCreateFence();

	//Done
	return true;
}

bool VulkanCommandQueue::SubmitCommandBuffersArray(EngineAPI::Graphics::RenderCommandBuffer* cmdBuffers, uint32_t cmdBuffersCount)
{
	//TODO

	//Done
	return true;
}


bool VulkanCommandQueue::SubmitCommandBuffersArrayAndWait(EngineAPI::Graphics::RenderCommandBuffer* cmdBuffers, uint32_t cmdBuffersCount)
{
	//TODO

	//VkFence fence; //Signalled when cmd buffer has finished executing
	//vkCreateFence();

	//Done
	return true;
}