#include "VulkanCommandBufferPool.h"

using namespace EngineAPI::Graphics::Platform;

bool VulkanCommandBufferPool::InitVKCommandBufferPool(VkDevice* vulkanLogicalDevice, uint32_t vkQueueFamilyIndex,
	bool shouldAllowIndividualCmdBufferResets, bool isTransientPool)
{
	//Cache logical device
	cachedVkLogicalDeviceHandle = *vulkanLogicalDevice;

	//Cache state
	canIndividuallyResetCommandBuffers = shouldAllowIndividualCmdBufferResets;
	isCmdPoolTransient = isTransientPool;
	cachedVkQueueFamilyIndexForSubmission = vkQueueFamilyIndex;

	//Flags for init. 
	cachedVkCommandPoolFlags = 0;
	if (isTransientPool)
		cachedVkCommandPoolFlags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	if (shouldAllowIndividualCmdBufferResets)
		cachedVkCommandPoolFlags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	//Creation struct
	VkCommandPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.queueFamilyIndex = cachedVkQueueFamilyIndexForSubmission;
	createInfo.flags = cachedVkCommandPoolFlags;

	//Create pool....
	VkResult result = vkCreateCommandPool(cachedVkLogicalDeviceHandle,
		&createInfo, nullptr, &vkCommandPoolHandle);
	if (result != VK_SUCCESS)
	{
		//Error:
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommandBufferPool: Error when creating Vulkan command pool\n");
		return false;
	}

	//Done
	return true;
}

void VulkanCommandBufferPool::Shutdown()
{
	vkDestroyCommandPool(cachedVkLogicalDeviceHandle, vkCommandPoolHandle, nullptr);
}

bool VulkanCommandBufferPool::ResetCommandBufferPool(bool doReleaseCommandBuffersBackToPool)
{
	VkResult result = vkResetCommandPool(cachedVkLogicalDeviceHandle, vkCommandPoolHandle,
		doReleaseCommandBuffersBackToPool ? VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT : NULL);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommandBufferPool: Error when resetting command pool\n");
		return false;
	}
	return true;
}

bool VulkanCommandBufferPool::GetVKCommandBufferFromPool(bool isPrimaryCmdBuffer, VkCommandBuffer* vkCommandBufferOut)
{
	//Alloc structure
	VkCommandBufferAllocateInfo commandBufferAllocInfo = {};
	commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocInfo.pNext = nullptr;
	commandBufferAllocInfo.level = isPrimaryCmdBuffer ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	commandBufferAllocInfo.commandPool = vkCommandPoolHandle;
	commandBufferAllocInfo.commandBufferCount = 1;

	//Create command buffer
	VkResult result = vkAllocateCommandBuffers(cachedVkLogicalDeviceHandle, &commandBufferAllocInfo, vkCommandBufferOut);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommandBufferPool: Error allocating VK command buffer.\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanCommandBufferPool::GetVKCommandBuffersArrayFromPool(uint32_t count, bool isAllPrimaryCmdBuffer, VkCommandBuffer* vkCommandBufferArrayOut)
{
	//Alloc structure
	VkCommandBufferAllocateInfo commandBufferAllocInfo = {};
	commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocInfo.pNext = nullptr;
	commandBufferAllocInfo.level = isAllPrimaryCmdBuffer ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	commandBufferAllocInfo.commandPool = vkCommandPoolHandle;
	commandBufferAllocInfo.commandBufferCount = count;

	//Create command buffer
	VkResult result = vkAllocateCommandBuffers(cachedVkLogicalDeviceHandle, &commandBufferAllocInfo, vkCommandBufferArrayOut);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommandBufferPool: Error allocating VK command buffers array.\n");
		return false;
	}

	//Done
	return true;
}

//*********************************************************************************************
//*********************************************************************************************
//**************************************DEPRECIATED********************************************
//*********************************************************************************************
//*********************************************************************************************

/*
EngineAPI::Graphics::RenderCommandBuffer* VulkanCommandBufferPool::AllocCommandBuffer(bool isPrimaryCmdBuffer)
{
	//Alloc cmd buffer
	EngineAPI::Graphics::RenderCommandBuffer* cmdBuffer = GE_NEW EngineAPI::Graphics::RenderCommandBuffer();
	if (cmdBuffer)
	{
		//Init
		if (!cmdBuffer->InitVKCommandBuffer(&cachedVkLogicalDeviceHandle, &vkCommandPoolHandle, canIndividuallyResetCommandBuffers, isPrimaryCmdBuffer))
		{
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommandBufferPool: Error when allocating render command object\n");
			cmdBuffer->Shutdown();
			delete cmdBuffer;
			return nullptr;
		}

	}
	else
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommandBufferPool: Error allocating EngineAPI::Graphics::RenderCommandBuffer!\n");
		return nullptr;
	}
	
	//Return pointer to cmd pool wrapper (heap)
	return cmdBuffer;
}
*/

/*
EngineAPI::Graphics::RenderCommandBuffer* VulkanCommandBufferPool::AllocCommandBuffersArray(uint32_t cmdBuffersCount, bool isAllPrimaryCmdBuffers)
{
	//Error check
	if (cmdBuffersCount == 0)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommandBufferPool: AllocCommandBuffersArray() -> Count == 0\n");
		return nullptr;

	}

	//Alloc buffers
	EngineAPI::Graphics::RenderCommandBuffer* cmdBuffers = GE_NEW EngineAPI::Graphics::RenderCommandBuffer[cmdBuffersCount];
	if (cmdBuffers)
	{
		//For each, init
		for (unsigned i = 0; i < cmdBuffersCount; i++)
		{
			if (!cmdBuffers[i].InitVKCommandBuffer(&cachedVkLogicalDeviceHandle, &vkCommandPoolHandle, canIndividuallyResetCommandBuffers, isAllPrimaryCmdBuffers))
			{
				//Error
				EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommandBufferPool: AllocCommandBuffersArray() -> Error when allocating render command object\n");
				return nullptr;
			}
		}
	}
	else
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommandBufferPool: Error allocating array of EngineAPI::Graphics::RenderCommandBuffer!\n");
		return nullptr;;
	}

	//Return pointer to first element in buffers array
	return &cmdBuffers[0];
}
*/

/*
EngineAPI::Graphics::RenderCommandBuffer* VulkanCommandBufferPool::AllocCommandBuffersArray(uint32_t cmdBuffersCount, bool* isPrimaryCmdBuffersArray)
{
	//Error check
	if (cmdBuffersCount == 0)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommandBufferPool: AllocCommandBuffersArray() -> Count == 0\n");
		return nullptr;
	}
	if (!isPrimaryCmdBuffersArray)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommandBufferPool: AllocCommandBuffersArray() -> isPrimaryCmdBuffersArray == NULL\n");
		return nullptr;
	}

	//Alloc buffers
	EngineAPI::Graphics::RenderCommandBuffer* cmdBuffers = GE_NEW EngineAPI::Graphics::RenderCommandBuffer[cmdBuffersCount];
	if (cmdBuffers)
	{
		//For each, init. Note: We have the levels flags (primary/secondary buffer) passed 
		//as an array - one entry per cmd buffer returned (hopefully...)
		for (unsigned i = 0; i < cmdBuffersCount; i++)
		{
			if (!cmdBuffers[i].InitVKCommandBuffer(&cachedVkLogicalDeviceHandle, &vkCommandPoolHandle, canIndividuallyResetCommandBuffers, isPrimaryCmdBuffersArray[i]))
			{
				//Error
				EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommandBufferPool: AllocCommandBuffersArray() -> Error when allocating render command object\n");
				return nullptr;
			}
		}
	}
	else
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanCommandBufferPool: Error allocating array of EngineAPI::Graphics::RenderCommandBuffer!\n");
		return nullptr;;
	}

	//Return pointer to first element in buffers array
	return &cmdBuffers[0];
}
*/