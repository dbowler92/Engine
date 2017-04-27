//VulkanCommandQueue.h
//Created 24/04/17
//Created By Daniel Bowler
//
//Abstracts VkQueue

#pragma once

//Base
#include "../Common/CommonCommandQueue.h"

//Vulkan header
#include <vulkan\vulkan.h>

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanCommandQueue : public EngineAPI::Graphics::Interface::CommonCommandQueue
			{
			public:
				VulkanCommandQueue() {};
				virtual ~VulkanCommandQueue() = 0 {};

				//Override shutdown function
				void Shutdown() override;

				//Vulkan init
				bool InitVKQueue(VkDevice* logicalDevice, 
					uint32_t queueFamilyIndex, uint32_t queueIndex);

			public:
				//VkCommandBuffer submission.
				bool SubmitVKCommandBuffer(VkCommandBuffer* cmdBuffer);
				bool SubmitVKCommandBufferAndWait(VkCommandBuffer* cmdBuffer);

				//Takes an array of VkCommandBuffer and submits the work to this queue
				bool SubmitVKCommandBuffersArray(VkCommandBuffer* cmdBuffersArray, uint32_t cmdBuffersCount);
				bool SubmitVKCommandBuffersArrayAndWait(VkCommandBuffer* cmdBuffersArray, uint32_t cmdBuffersCount);

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
