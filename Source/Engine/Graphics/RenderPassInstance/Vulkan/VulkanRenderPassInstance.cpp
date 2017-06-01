#include "VulkanRenderPassInstance.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanRenderPassInstance::Shutdown()
{
	//Release command buffer
	if (vkRenderPassInstanceCmdBuffer)
	{
		if (!EngineAPI::Statics::VulkanStatics::CommandBufferReset(&vkRenderPassInstanceCmdBuffer, true))
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderPassInstance::Shutdown() - Could not reset command buffers\n");
		vkRenderPassInstanceCmdBuffer = VK_NULL_HANDLE;
	}
}

bool VulkanRenderPassInstance::InitVKRenderPassInstance()
{
	//Done
	return true;
}

bool VulkanRenderPassInstance::BuildVKRenderPassInstanceCommandBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Graphics::RenderPass* renderPass, EngineAPI::Graphics::Framebuffer* frameBuffer,
	UNorm32Colour colourBufferClearValue, float depthClearValue, uint32_t stencilClearValue, VkExtent2D renderAreaExtents)
{
	//if recalling (Eg: Due to changes to the clear colour info, we need to delete the old command
	//buffers)
	if (vkRenderPassInstanceCmdBuffer != VK_NULL_HANDLE)
	{
		if (!EngineAPI::Statics::VulkanStatics::CommandBufferReset(&vkRenderPassInstanceCmdBuffer, true))
		{
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderPassInstance::BuildVKRenderPassInstanceCommandBuffer() - Could not reset command buffers\n");
			return false;
		}
	}

	//Get command buffer
	if (!renderingDevice->GetGraphicsCommandQueueFamily()->GetCommandBufferPool(0).GetVKCommandBufferFromPool(true, &vkRenderPassInstanceCmdBuffer))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderPassInstance::BuildVKRenderPassInstanceCommandBuffer() - Could not get command buffer\n");
		return false;
	}

	//Bind command buffer
	assert(EngineAPI::Statics::VulkanStatics::CommandBufferBeginRecordingDefault(&vkRenderPassInstanceCmdBuffer));

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

	//
	//****Add drawing commands here****
	//

	//End of render pass instance recording
	vkCmdEndRenderPass(vkRenderPassInstanceCmdBuffer);

	//End reading in to this command buffer
	assert(EngineAPI::Statics::VulkanStatics::CommandBufferEndRecording(&vkRenderPassInstanceCmdBuffer));

	//Done
	return true;
}

bool VulkanRenderPassInstance::SubmitVKRenderPassInstanceCommandBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice)
{
	//Submit render pass instance to queue
	assert(renderingDevice->GetGraphicsCommandQueueFamily()->SubmitVKCommandBuffersToQueueDefault(0,
		&vkRenderPassInstanceCmdBuffer, 1, VK_NULL_HANDLE, true));

	//Done
	return true;
}