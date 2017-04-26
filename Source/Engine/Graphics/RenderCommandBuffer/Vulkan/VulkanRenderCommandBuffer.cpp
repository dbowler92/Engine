#include "VulkanRenderCommandBuffer.h"

using namespace EngineAPI::Graphics::Platform;

bool VulkanRenderCommandBuffer::InitVKCommandBuffer(VkDevice* renderingDevice, VkCommandPool* commandPool, 
	bool poolCreatedWithResetCmdBufferBit, bool isPrimaryCmdBuffer)
{
	//Cache rendering device handle && command pool handle used to create this buffer
	cachedVkDeviceHandle = *renderingDevice;
	cachedVkCommandPoolHandle = *commandPool;

	//Primary buffer?
	vkIsPrimaryBuffer = isPrimaryCmdBuffer;

	//Are we allowed to manually reset this buffer?
	parentPoolCreatedWithResetCommandBufferBit = poolCreatedWithResetCmdBufferBit;

	//Alloc structure
	VkCommandBufferAllocateInfo commandBufferAllocInfo = {};
	commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocInfo.pNext = nullptr;
	commandBufferAllocInfo.level = vkIsPrimaryBuffer ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	commandBufferAllocInfo.commandPool = cachedVkCommandPoolHandle;
	commandBufferAllocInfo.commandBufferCount = 1;

	//Create command buffer
	VkResult result = vkAllocateCommandBuffers(cachedVkDeviceHandle, &commandBufferAllocInfo, &vkCommandBufferHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderCommandBuffer: Error allocating command buffer.\n");
		return false;
	}

	//Done
	return true;
}

void VulkanRenderCommandBuffer::Shutdown()
{
	vkFreeCommandBuffers(cachedVkDeviceHandle, cachedVkCommandPoolHandle, 1, &vkCommandBufferHandle);
}

bool VulkanRenderCommandBuffer::ResetCommandBuffer(bool shouldReleaseMemoryToCommandPool)
{
	if (!parentPoolCreatedWithResetCommandBufferBit)
	{
		//Pool doesnt support this...
		EngineAPI::Debug::DebugLog::PrintWarningMessage("VulkanRenderCommandBuffer: Can not reset command buffer - Pool not created with flag VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT\n");
		return false;
	}

	VkResult result = vkResetCommandBuffer(vkCommandBufferHandle, 
		shouldReleaseMemoryToCommandPool ? VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : NULL);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderCommandBuffer: Error when resetting command buffer.\n");
		return false;
	}

	return true;
}

bool VulkanRenderCommandBuffer::BeginRecordingToCommandBuffer()
{
	//TODO: Inheritance info...
	VkCommandBufferInheritanceInfo inheritanceInfo = {};
	
	//Buffer begin reading info
	VkCommandBufferBeginInfo commandBufferBeginRecordingInfo = {};
	commandBufferBeginRecordingInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginRecordingInfo.pNext = nullptr;
	commandBufferBeginRecordingInfo.pInheritanceInfo = nullptr; //TODO
	commandBufferBeginRecordingInfo.flags = NULL; //TODO

	//Begin reading to this command buffer...
	VkResult result = vkBeginCommandBuffer(vkCommandBufferHandle, &commandBufferBeginRecordingInfo);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderCommandBuffer: Error when begining recording to command buffer.\n");
		return false;
	}

	return true;
}

bool VulkanRenderCommandBuffer::EndRecordingToCommandBuffer()
{
	VkResult result = vkEndCommandBuffer(vkCommandBufferHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderCommandBuffer: Error when ending recording to command buffer.\n");
		return false;
	}

	return true;
}