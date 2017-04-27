//VulkanRenderCommandBuffer.h
//Created 21/04/17
//Created by Daniel Bowler
//
//Represents the vulkan command buffer that will be sent to 
//the GPU for processing. Create one of these by calling command 
//buffer pool functions rather than creating (new) one manually. 

#pragma once

//Base class
#include "../Common/CommonRenderCommandBuffer.h"

//Vulkan header
#include <vulkan\vulkan.h>

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanRenderCommandBuffer : public EngineAPI::Graphics::Interface::CommonRenderCommandBuffer
			{
			public:
				VulkanRenderCommandBuffer() {};
				virtual ~VulkanRenderCommandBuffer() = 0 {};

				//Override shutdown function providing vulkan specific
				//implementation
				void Shutdown() override;

				//Resets the command buffer
				bool ResetCommandBuffer(bool shouldReleaseMemoryToCommandPool = false) override;

				//Captures vulkan rendering commands in to a reusable command
				//buffer
				bool BeginRecordingToCommandBuffer() override;
				bool EndRecordingToCommandBuffer() override;

			public:
				//Inits the command buffer (vk)
				bool InitVKCommandBuffer(VkDevice* renderingDevice, VkCommandPool* commandPool, 
					bool poolCreatedWithResetCmdBufferBit, bool isPrimaryCmdBuffer = true);

			public:
				//Returns the VK handle for this command buffer
				const VkCommandBuffer GetVKCommandBufferHandle() { return vkCommandBufferHandle; };

			protected:
				//Handle to the VK command buffer
				VkCommandBuffer vkCommandBufferHandle = NULL;

				//Is this a primary buffer
				bool vkIsPrimaryBuffer;

				//Does the pool this cmd buffer created within support indivudal 
				//resets -> VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
				bool parentPoolCreatedWithResetCommandBufferBit;

			protected:
				//Cache the device handle used to construct this 
				//VK command buffer
				VkDevice cachedVkDeviceHandle = NULL;

				//Cache the handle to the command pool used to allocate this buffer
				//from
				VkCommandPool cachedVkCommandPoolHandle;
			};
		};
	};
};