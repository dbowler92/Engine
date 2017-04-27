#include "VulkanCommandQueueFamily.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanCommandQueueFamily::Shutdown()
{
	if (queuesPrioritiesArray)
		delete[] queuesPrioritiesArray;

	if (commandQueuesArray)
	{
		for (int i = 0; i < commandQueuesCount; i++)
			commandQueuesArray[i].Shutdown();
		delete[] commandQueuesArray;
	}
}

bool VulkanCommandQueueFamily::InitVulkanQueueFamily(VkDevice* logicalDevice, QueueFamilySupport role,
	uint32_t vkQueueFamilyIndex, unsigned queueCount, float* queuesPriorties, 
	VkDeviceQueueCreateInfo* creationStructOut)
{
	if (!creationStructOut)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsCommandQueueFamily: Error: VkDeviceQueueCreateInfo was NULL\n");
		return false;
	}

	//Cache info
	queueFamilyRole = role;
	commandQueuesCount = queueCount;
	this->vkQueueFamilyIndex = vkQueueFamilyIndex;
	this->queuesPrioritiesArray = GE_NEW float[queueCount];
	memcpy(this->queuesPrioritiesArray, queuesPriorties, sizeof(float) * queueCount);

	//Alloc queues - init InitVulkanQueues() called by the VulkanRenderDevice after device creation
	commandQueuesArray = GE_NEW CommandQueue[queueCount];

	//Fillout struct which tells the device how we wish to enable this particular
	//queue family
	creationStructOut->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	creationStructOut->pNext = nullptr; //Maybe overriden by VulkanRenderDevice if more than one queue family will be enabled
	creationStructOut->flags = 0;
	creationStructOut->queueFamilyIndex = vkQueueFamilyIndex;
	creationStructOut->queueCount = commandQueuesCount;
	creationStructOut->pQueuePriorities = queuesPrioritiesArray;
	
	//Done
	return true;
}

bool VulkanCommandQueueFamily::InitVulkanQueues(VkDevice* logicalDevice)
{
	//Init each queue in this familyB
	for (int i = 0; i < commandQueuesCount; i++)
	{
		if (!commandQueuesArray[i].InitVKQueue(logicalDevice, vkQueueFamilyIndex, i))
			return false;
	}

	//Done
	return true;
}