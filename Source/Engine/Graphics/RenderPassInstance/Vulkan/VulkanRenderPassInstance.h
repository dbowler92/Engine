//VulkanRenderPassInstance.h
//Created 01/06/17
//Created By Daniel Bowler
//
//Represents a single Vulkan render pass instance. We will use it to a) Clear the render attachments
//b) fill the command buffer with rendering commands (or reuse them) and c) then submit the command
//buffer for processing.

#pragma once

//Globals
#include "../../../Main/EngineStd.h"

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Device
#include "../../Device/RenderDevice.h"

//Vulkan static functions
#include "../../../Statics/Vulkan/VulkanStatics.h"

//Frame buffer and render pass
#include "../../../Graphics/RenderPass/RenderPass.h"
#include "../../../Graphics/Framebuffer/Framebuffer.h"

//Vulkan header
#include <vulkan\vulkan.h>

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanRenderPassInstance
			{
			public:
				VulkanRenderPassInstance() {};
				virtual ~VulkanRenderPassInstance() = 0 {};

				//Cleansup
				void Shutdown();

				//Inits the render pass instance.
				bool InitVKRenderPassInstance(EngineAPI::Graphics::RenderDevice* renderingDevice);

				//Resets the render pass command buffer
				bool ResetVKRenderPassCommandBuffer();

				//Begins reading in to the render pass instance command buffer 
				bool BeginVKRenderPassInstanceCommandBufferRecording();

				//Adds render pass (instance) begin commands to the command buffer -> Eg: Clearing
				//the render attachments
				bool InsertVKRenderPassCommandBufferBeginRenderPassCommands(EngineAPI::Graphics::RenderDevice* renderingDevice,
					EngineAPI::Graphics::RenderPass* renderPass, EngineAPI::Graphics::Framebuffer* frameBuffer,
					UNorm32Colour colourBufferClearValue, float depthClearValue, uint32_t stencilClearValue, VkExtent2D renderAreaExtents);

				//Adds the vkCmdEndRenderPass() commands to the command buffer
				bool InsertVKRenderPassCommandBufferEndRenderPassCommands();

				//Ends recording to the command buffer
				bool EndVKRenderPassInstanceCommandBufferRecording();

				//Submits the command buffer for processing
				bool SubmitVKRenderPassInstanceCommandBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice);

			protected:
				//Render pass instance command buffer
				VkCommandBuffer vkRenderPassInstanceCmdBuffer = VK_NULL_HANDLE;
			};
		};
	};
};
