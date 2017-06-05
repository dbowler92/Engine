//VulkanDescriptorPool.h
//Created 05/06/17
//Created By Daniel Bowler
//
//Wrapper around VkDescriptorPool

#pragma once

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Device
#include "../../Device/RenderDevice.h"

//Vulkan header
#include <vulkan/vulkan.h>

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanDescriptorPool
			{
			public:
				VulkanDescriptorPool() {};
				virtual ~VulkanDescriptorPool() = 0 {};

				//Shutdown the descriptor object
				void Shutdown();

				//Inits the descriptor pool
				bool InitVKDescriptorPools(EngineAPI::Graphics::RenderDevice* renderingDevice, 
					VkDescriptorPoolSize* pools, uint32_t poolsCount, bool doInitWithCreateFreeDescriptorSetBitFlag);

			protected:
				//Vk handle representing the descriptor pool
				VkDescriptorPool vkDescriptorPoolHandle = VK_NULL_HANDLE;

				//Cached device that ownes this desc pool
				VkDevice cachedVKLogicalDevice = VK_NULL_HANDLE;
			};
		};
	};
};
