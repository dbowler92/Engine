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

//What are the queues job within this family?
enum QueueFamilySupport
{
	QUEUE_FAMILY_SUPPORT_NULL,                //Not inited yet.
	QUEUE_FAMILY_SUPPORT_GRAPHICS,
	QUEUE_FAMILY_SUPPORT_PRESENTATION,
	QUEUE_FAMILY_SUPPORT_GRAPHICS_AND_PRESENTATION,
	QUEUE_FAMILY_SUPPORT_COMPUTE,
	QUEUE_FAMILY_SUPPORT_DMA_TRANSFER,
	QUEUE_FAMILY_SUPPORT_SPARSE,
	QUEUE_FAMILY_SUPPORT_ALL_GRAPHICS_WORK    //D3D11 etc -> Probably just have one queue family with one queue doing everything...
};

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

				//Override shutdown function
				void Shutdown();

			public:
				//Inits the Vulkan queue family. Once the device has been created, call
				//InitVulkanQueues() which will cache info on each seperate queue within
				//this family
				bool InitVulkanQueueFamily(VkDevice* logicalDevice, QueueFamilySupport role,
					uint32_t vkQueueFamilyIndex, unsigned queueCount, float* queuesPriorties, 
					VkDeviceQueueCreateInfo* creationStructOut);

				//Once the device has been created, we can init each queue within this
				//family. 
				bool InitVulkanQueues(VkDevice* logicalDevice);
				
			public:
				//Returns the queues & number of them
				CommandQueue* GetCommandQueues() { return commandQueuesArray; };
				unsigned GetCommandQueueCount() { return commandQueuesCount; };

				//Command queue family role
				QueueFamilySupport GetCommandQueueFamilyRole() { return queueFamilyRole; };

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

			protected:
				//Will contain and manage a set of command queues
				CommandQueue* commandQueuesArray = nullptr;

				//Info on this queue family
				QueueFamilySupport queueFamilyRole = QUEUE_FAMILY_SUPPORT_NULL;
				unsigned commandQueuesCount = 0;
			};
		};
	};
};
