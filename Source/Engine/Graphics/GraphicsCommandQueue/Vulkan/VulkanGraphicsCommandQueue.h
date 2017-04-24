//VulkanGraphicsQueue.h
//Created 24/04/17
//Created By Daniel Bowler
//
//Abstracts VkQueue

#pragma once

//Base
#include "../Common/CommonGraphicsCommandQueue.h"

//Vulkan header
#include <vulkan\vulkan.h>

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanGraphicsCommandQueue : public EngineAPI::Graphics::Interface::CommonGraphicsCommandQueue
			{
			public:
				VulkanGraphicsCommandQueue() {};
				virtual ~VulkanGraphicsCommandQueue() = 0 {};

				//Override shutdown function
				void Shutdown() override;

				//Vulkan init
				bool InitVKQueue(VkDevice* logicalDevice, 
					uint32_t queueFamilyIndex, uint32_t queueIndex);

			protected:
				//Vulkan queue handle
				VkQueue vkQueueHandle = NULL;
			};
		};
	};
};
