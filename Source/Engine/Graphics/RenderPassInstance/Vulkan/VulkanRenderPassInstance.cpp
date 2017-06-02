#include "VulkanRenderPassInstance.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanRenderPassInstance::Shutdown()
{
	//Destroy semaphore
	if (vkRenderPassInstanceCmdBufferSignalSemaphore)
		vkDestroySemaphore(cachedVKLogicalDevice, vkRenderPassInstanceCmdBufferSignalSemaphore, nullptr);

	//Release command buffer
	if (vkRenderPassInstanceCmdBuffer)
	{
		if (!EngineAPI::Statics::VulkanStatics::CommandBufferReset(&vkRenderPassInstanceCmdBuffer, true))
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderPassInstance::Shutdown() - Could not reset command buffers\n");
		vkRenderPassInstanceCmdBuffer = VK_NULL_HANDLE;
	}
}

bool VulkanRenderPassInstance::InitVKRenderPassInstance(EngineAPI::Graphics::RenderDevice* renderingDevice)
{
	//Cache device
	cachedVKLogicalDevice = renderingDevice->GetVKLogicalDevice();

	//Get command buffer
	if (!renderingDevice->GetGraphicsCommandQueueFamily()->GetCommandBufferPool(0).GetVKCommandBufferFromPool(true, &vkRenderPassInstanceCmdBuffer))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderPassInstance::BuildVKRenderPassInstanceCommandBuffer() - Could not get command buffer\n");
		return false;
	}

	//Create semaphore
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = nullptr;
	semaphoreCreateInfo.flags = 0;
	VkResult result = vkCreateSemaphore(cachedVKLogicalDevice, &semaphoreCreateInfo, nullptr, &vkRenderPassInstanceCmdBufferSignalSemaphore);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderPassInstance::InitVKRenderPassInstance() Error: Could not create semaphore\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanRenderPassInstance::InsertVKRenderPassCommandBufferBeginRenderPassCommands(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Graphics::RenderPass* renderPass, EngineAPI::Graphics::Framebuffer* frameBuffer,
	UNorm32Colour colourBufferClearValue, float depthClearValue, uint32_t stencilClearValue, VkExtent2D renderAreaExtents)
{
	//Early return if we are not allowed to make changes to this command buffer
	if (!renderPassInstanceDirtyFlag)
		return true;

	//Clear values
	VkClearValue clearValues[2];

	//Colour buffer
	clearValues[0].color.float32[0] = colourBufferClearValue.R;
	clearValues[0].color.float32[1] = colourBufferClearValue.G;
	clearValues[0].color.float32[2] = colourBufferClearValue.B;
	clearValues[0].color.float32[3] = colourBufferClearValue.A;
	
	//Depth stencil buffer
	clearValues[1].depthStencil.depth = depthClearValue;
	clearValues[1].depthStencil.stencil = stencilClearValue;

	//Create render pass instance -> This will be used to record a 
	//begin render pass command in to this buffer
	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.pNext = nullptr;
	renderPassInfo.renderPass = renderPass->GetVKRenderPassHandle();
	renderPassInfo.framebuffer = frameBuffer->GetVKFramebufferHandle();
	renderPassInfo.renderArea.offset.x = 0;
	renderPassInfo.renderArea.offset.y = 0;
	renderPassInfo.renderArea.extent.width = renderAreaExtents.width;
	renderPassInfo.renderArea.extent.height = renderAreaExtents.height;
	renderPassInfo.clearValueCount = 2;
	renderPassInfo.pClearValues = clearValues;

	//Start recording the render pass instance
	vkCmdBeginRenderPass(vkRenderPassInstanceCmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	//Done
	return true;
}

bool VulkanRenderPassInstance::InsertVKRenderPassCommandBufferEndRenderPassCommands()
{
	//Early return if we are not allowed to make changes to this command buffer
	if (!renderPassInstanceDirtyFlag)
		return true;

	//End of render pass instance recording
	vkCmdEndRenderPass(vkRenderPassInstanceCmdBuffer);

	//Done
	return true;
}

bool VulkanRenderPassInstance::ResetVKRenderPassCommandBuffer()
{
	//Early return if we are not allowed to make changes to this command buffer
	if (!renderPassInstanceDirtyFlag)
		return true;

	if (vkRenderPassInstanceCmdBuffer != VK_NULL_HANDLE)
	{
		if (!EngineAPI::Statics::VulkanStatics::CommandBufferReset(&vkRenderPassInstanceCmdBuffer, false))
		{
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderPassInstance::ResetVKRenderPassCommandBuffer() - Could not reset command buffers\n");
			return false;
		}
	}

	//Done
	return true;
}

bool VulkanRenderPassInstance::BeginVKRenderPassInstanceCommandBufferRecording()
{
	//Early return if we are not allowed to make changes to this command buffer
	if (!renderPassInstanceDirtyFlag)
		return true;

	assert(EngineAPI::Statics::VulkanStatics::CommandBufferBeginRecordingDefault(&vkRenderPassInstanceCmdBuffer));

	//Done
	return true;
}

bool VulkanRenderPassInstance::EndVKRenderPassInstanceCommandBufferRecording()
{
	//Early return if we are not allowed to make changes to this command buffer
	if (!renderPassInstanceDirtyFlag)
		return true;

	//End reading in to this command buffer
	assert(EngineAPI::Statics::VulkanStatics::CommandBufferEndRecording(&vkRenderPassInstanceCmdBuffer));

	//Done
	return true;
}

bool VulkanRenderPassInstance::SubmitVKRenderPassInstanceCommandBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice, bool doUseInternalSignalSemaphore)
{
	if (doUseInternalSignalSemaphore)
	{
		VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		//Submit description
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.pWaitDstStageMask = &submitPipelineStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &vkRenderPassInstanceCmdBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &vkRenderPassInstanceCmdBufferSignalSemaphore;

		//Submit work.
		assert(renderingDevice->GetGraphicsCommandQueueFamily()->SubmitVKCommandBuffersToQueue(0, &submitInfo, 1, VK_NULL_HANDLE, true));
	}
	else
	{
		//Submit render pass instance to queue
		assert(renderingDevice->GetGraphicsCommandQueueFamily()->SubmitVKCommandBuffersToQueueDefault(0,
			&vkRenderPassInstanceCmdBuffer, 1, VK_NULL_HANDLE, true));
	}

	//Done
	return true;
}