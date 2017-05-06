//VulkanCommandQueue.h
//Created 24/04/17
//Created By Daniel Bowler
//
//Abstracts VkQueue

#pragma once

//Vulkan header
#include <vulkan\vulkan.h>

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanCommandQueue
			{
			public:
				VulkanCommandQueue() {};
				virtual ~VulkanCommandQueue() = 0 {};

				//Override shutdown function
				void Shutdown();

				//Vulkan init
				bool InitVKQueue(VkDevice* logicalDevice, 
					uint32_t queueFamilyIndex, uint32_t queueIndex);

			public:
				//Submits VkCommandBuffer(s) to the queue.
				bool SubmitVKCommandBuffers(VkSubmitInfo* submitInfos, uint32_t submitInfosCount,
					const VkFence& optionalFence = VK_NULL_HANDLE, bool doWaitOnQueueIdle = true);

				//Default command buffer submission (we generate a default VkSubmitInfo struct)
				bool SubmitVKCommandBuffersDefault(VkCommandBuffer* cmdBuffers, uint32_t cmdBufferCount,
					const VkFence& optionalFence = VK_NULL_HANDLE, bool doWaitOnQueueIdle = true);

				//Waits on this queue to become idle / finish its batch
				bool BlockUntilVKQueueIdle();

			protected:
				//Vulkan queue handle
				VkQueue vkQueueHandle = NULL;

			protected:
				//Cached data
				VkDevice cachedVkDeviceHandle = NULL;
			};
		};
	};
};
