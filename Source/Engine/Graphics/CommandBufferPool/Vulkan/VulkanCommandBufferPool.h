//VulkanCommandBufferPool.h
//Created 24/04/17
//Created by Daniel Bowler
//
//Vulkan inplementation for command buffer pools - VkCommandPool

#pragma once

//Base
#include "../Common/CommonCommandBufferPool.h"

//Vulkan static functions
#include "../../Statics/Vulkan/VulkanStatics.h"

//Vulkan header
#include <vulkan\vulkan.h>

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanCommandBufferPool : public EngineAPI::Graphics::Interface::CommonCommandBufferPool
			{
			public:
				VulkanCommandBufferPool() {};
				virtual ~VulkanCommandBufferPool() = 0 {};

				//Override shutdown function
				void Shutdown() override;

			public:
				//VK init function
				bool InitVKCommandBufferPool(VkDevice* vulkanLogicalDevice, uint32_t vkQueueFamilyIndex,
					bool shouldAllowIndividualCmdBufferResets = true, bool isTransientPool = false);

			public:
				//Resets all command buffers
				bool ResetCommandBufferPool(bool doReleaseCommandBuffersBackToPool = false) override;

				//Returns a VkCommandBuffer(s) from the command pool
				bool GetVKCommandBufferFromPool(bool isPrimaryCmdBuffer, VkCommandBuffer* vkCommandBufferOut);
				bool GetVKCommandBuffersArrayFromPool(uint32_t count, bool isAllPrimaryCmdBuffer, VkCommandBuffer* vkCommandBufferArrayOut);

			protected:
				//Handle to the vulkan created command pool
				VkCommandPool vkCommandPoolHandle = NULL;

				//Cache the logical device - needed at shutdown
				VkDevice cachedVkLogicalDeviceHandle = NULL;

				//Cache the queue family index that command buffers from this pool
				//will be expected to be sent too
				uint32_t cachedVkQueueFamilyIndexForSubmission = 0;
				//Cache the flags this command pool has been created with
				uint32_t cachedVkCommandPoolFlags = 0;

				//Can we manually reset command buffers within this pool?
				bool canIndividuallyResetCommandBuffers;

				//Is pool transient - short lived?
				bool isCmdPoolTransient;
			};
		};
	};
};