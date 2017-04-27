//VulkanQueueFamily.h
//Created 24/04/17
//Created By Daniel Bowler
//
//Represents a Vulkan queue family. Will contain a set of VkQueues (per queue we create)

#pragma once

//Base
#include "../Common/CommonCommandQueueFamily.h"

//Vulkan header
#include <vulkan\vulkan.h>

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanCommandQueueFamily : public EngineAPI::Graphics::Interface::CommonCommandQueueFamily
			{
			public:
				VulkanCommandQueueFamily() {};
				virtual ~VulkanCommandQueueFamily() = 0 {};

				//Override shutdown function
				void Shutdown() override;

			public:
				//Returns the vulkan assigned index for this queue family. 
				uint32_t GetVKQueueFamilyIndex() { return vkQueueFamilyIndex; };

			public:
				//Inits the Vulkan queue family. Once the device has been created, call
				//InitVulkanQueues() which will cache info on each seperate queue within
				//this family
				bool InitVulkanQueueFamily(QueueFamilySupport role,
					uint32_t vkQueueFamilyIndex, unsigned queueCount, float* queuesPriorties, 
					VkDeviceQueueCreateInfo* creationStructOut);

				//Once the device has been created, we can init each queue within this
				//family. 
				bool InitVulkanQueues(VkDevice* logicalDevice);

			protected:
				//VK info
				uint32_t vkQueueFamilyIndex = 0;
				float* queuesPrioritiesArray = nullptr;
			};
		};
	};
};
