//VulkanCommandBufferPool.h
//Created 24/04/17
//Created by Daniel Bowler
//
//Vulkan inplementation for command buffer pools - VkCommandPool

#pragma once

//Base
#include "../Common/CommonCommandBufferPool.h"

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
				bool InitVKCommandBufferPool(VkDevice* vulkanLogicalDevice, 
					VkCommandPoolCreateInfo* poolCreateInfo);

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
			};
		};
	};
};