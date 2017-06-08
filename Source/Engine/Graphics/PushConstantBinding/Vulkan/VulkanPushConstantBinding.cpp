#include "VulkanPushConstantBinding.h"

//Complete forward declarations
#include "../../../Graphics/GraphicsPipelineLayout/GraphicsPipelineLayout.h"

//Statics -> Wraps the command to update constants
#include "../../../Statics/Vulkan/VulkanStatics.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanPushConstantBinding::Shutdown()
{
	//Cleanup command buffer
	assert(EngineAPI::Statics::VulkanStatics::CommandBufferReset(&vkPushConstantUpdateCmdBuffer, true));
}

bool VulkanPushConstantBinding::InitVKPushConstantBinding(EngineAPI::Graphics::RenderDevice* renderingDevice,
	VkShaderStageFlags shaderStagesFlag, uint32_t offset, uint32_t sizeBytes)
{
	//Fill vk struct
	vkPushConstantBindingRange.stageFlags = shaderStagesFlag;
	vkPushConstantBindingRange.offset = offset;
	vkPushConstantBindingRange.size = sizeBytes;

	//Create the command buffer
	if (!renderingDevice->GetGraphicsCommandQueueFamily()->GetCommandBufferPool(0).GetVKCommandBufferFromPool(true, &vkPushConstantUpdateCmdBuffer))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanPushConstantBinding::InitVKPushConstantBinding() Error: Could not get command buffer\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanPushConstantBinding::UpdateVKPushConstantBindingData(EngineAPI::Graphics::CommandQueue* commandQueue,
	EngineAPI::Graphics::GraphicsPipelineLayout* pipelineLayout, void* data)
{
	assert(commandQueue != nullptr);
	assert(pipelineLayout != nullptr);
	assert(data != nullptr);

	//Reset command buffer
	assert(EngineAPI::Statics::VulkanStatics::CommandBufferReset(&vkPushConstantUpdateCmdBuffer, true));

	//Begin reading to command buffer
	assert(EngineAPI::Statics::VulkanStatics::CommandBufferBeginRecordingDefault(&vkPushConstantUpdateCmdBuffer));

	//Add update command
	EngineAPI::Statics::VulkanCommands::VKCMD_UpdatePushConstants(vkPushConstantUpdateCmdBuffer, pipelineLayout->GetVKPipelineLayoutHandle(),
		vkPushConstantBindingRange.stageFlags, vkPushConstantBindingRange.offset, vkPushConstantBindingRange.size, data);

	//Finish recording
	assert(EngineAPI::Statics::VulkanStatics::CommandBufferEndRecording(&vkPushConstantUpdateCmdBuffer));

	//Submit info.
	assert(commandQueue->SubmitVKCommandBuffersDefault(&vkPushConstantUpdateCmdBuffer, 1, VK_NULL_HANDLE, true));

	//Done
	return true;
}