//VulkanQueueFamily.h
//Created 24/04/17
//Created By Daniel Bowler
//
//Represents a Vulkan queue family. Will contain a set of VkQueues (per queue we create)

#pragma once

//Vulkan header
#include <vulkan\vulkan.h>

//Manages specific command queues
#include "../../CommandQueue/CommandQueue.h"

//Debug
#include "../../../Debug/Log/DebugLog.h"

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanCommandQueueFamily 
			{
			public:
				VulkanCommandQueueFamily() {};
				virtual ~VulkanCommandQueueFamily() = 0 {};

				//Cleansup
				void Shutdown();

			public:
				//Inits the Vulkan queue family. Once the device has been created, call
				//InitVulkanQueues() which will cache info on each seperate queue within
				//this family
				bool InitVKQueueFamily(VkDevice* logicalDevice, 
					uint32_t vkQueueFamilyIndex, VkQueueFlags queueSupportFlag, unsigned queueCount, float* queuesPriorties,
					VkDeviceQueueCreateInfo* creationStructOut);

				//Once the device has been created, we can init each queue within this
				//family. 
				bool InitVKQueues(VkDevice* logicalDevice);
				
			public:
				//Returns the queues & number of them
				CommandQueue* GetCommandQueues() { return commandQueuesArray; };
				unsigned GetCommandQueueCount() { return commandQueuesCount; };

				//Command queue family support flag
				VkQueueFlags GetVKCommandQueueSupportFlags() { return vkQueueFamilySupportFlags; };

			public:
				//Submits VKCommandBuffer(s) to this queue family (and then queue by index) for processing
				//NOTE: SubmitVKCommandBuffersToQueueDefault() will generate a VkSubmitInfos
				//structure with a default setting for us to use when sending the VkCommandBuffer(s)
				//to the queue for processing. 
				bool SubmitVKCommandBuffersToQueue(uint32_t queueIndex, 
					VkSubmitInfo* submitInfos, uint32_t submitInfosCount,
					const VkFence& optionalFence = VK_NULL_HANDLE, bool doWaitOnQueueIdle = true);
				bool SubmitVKCommandBuffersToQueueDefault(uint32_t queueIndex,
					VkCommandBuffer* cmdBuffers, uint32_t cmdBufferCount,
					const VkFence& optionalFence = VK_NULL_HANDLE, bool doWaitOnQueueIdle = true);

			public:
				//Returns the vulkan assigned index for this queue family. 
				uint32_t GetVKQueueFamilyIndex() { return vkQueueFamilyIndex; };

			protected:
				//VK info
				uint32_t vkQueueFamilyIndex = 0;
				float* queuesPrioritiesArray = nullptr;
				VkQueueFlags vkQueueFamilySupportFlags;
				
			protected:
				//Will contain and manage a set of command queues
				CommandQueue* commandQueuesArray = nullptr;

				//Info on this queue family
				unsigned commandQueuesCount = 0;
			};
		};
	};
};
