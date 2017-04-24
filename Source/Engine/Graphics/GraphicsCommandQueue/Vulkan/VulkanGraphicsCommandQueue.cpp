#include "VulkanGraphicsCommandQueue.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanGraphicsCommandQueue::Shutdown()
{
	//?
}

bool VulkanGraphicsCommandQueue::InitVKQueue(VkDevice* logicalDevice, uint32_t queueFamilyIndex, uint32_t queueIndex)
{
	//Cache VkQueue handle
	vkGetDeviceQueue(*logicalDevice, queueFamilyIndex, queueIndex, &vkQueueHandle);

	//Done
	return true;
}