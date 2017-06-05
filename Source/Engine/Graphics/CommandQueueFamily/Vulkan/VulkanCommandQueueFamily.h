//VulkanQueueFamily.h
//Created 24/04/17
//Created By Daniel Bowler
//
//Represents a Vulkan queue family. Will contain a set of VkQueues (per queue we create)

#pragma once

//Vulkan header
#include <vulkan\vulkan.h>

//Vector
#include <vector>

//Manages specific command queues
#include "../../CommandQueue/CommandQueue.h"

//Manages an array of command buffer pools - they are tied to 
//queue family indicies.
#include "../../CommandBufferPool/CommandBufferPool.h"

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
				
				//Creates a new command buffer pool for this queue family
				bool CreateVKCommandBufferPool(VkDevice* logicalDevice, 
					bool doAllowManualBufferResets, bool isTransientBufferPool);

			public:
				//Returns the queues & number of them
				CommandQueue* GetCommandQueues() { return commandQueuesArray; };
				CommandQueue* GetCommandQueueAtIndex(int index) { return &commandQueuesArray[index]; };
				unsigned GetCommandQueueCount() { return commandQueuesCount; };

				//Command queue family support flag
				VkQueueFlags GetVKCommandQueueSupportFlags() { return vkQueueFamilySupportFlags; };

				//Command buffer pools
				std::vector<CommandBufferPool>& GetCommandBufferPoolsArray() { return commandBufferPoolsArray; };
				CommandBufferPool& GetCommandBufferPool(uint32_t index = 0) { return commandBufferPoolsArray[index]; };
				uint32_t GetCommandBufferPoolsCount() { return commandBufferPoolsArray.size(); };

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
				//Set of individual queues
				CommandQueue* commandQueuesArray = nullptr;
				uint32_t commandQueuesCount = 0;

				//Command buffer pools for this queue family. 
				std::vector<CommandBufferPool> commandBufferPoolsArray;
			};
		};
	};
};
