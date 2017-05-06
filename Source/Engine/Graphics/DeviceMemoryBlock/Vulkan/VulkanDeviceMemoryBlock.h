//VulkanDeviceMemoryBlock.h
//Created 04/05/17
//Created By Daniel Bowler
//
//Represents an individual piece of device memory (eg a VKImage, VkBuffer etc)

#pragma once

//Base
#include "../Common/CommonDeviceMemoryBlock.h"

//Vulkan header
#include <vulkan\vulkan.h>

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanDeviceMemoryBlock : public EngineAPI::Graphics::Interface::CommonDeviceMemoryBlock
			{
			public:
				VulkanDeviceMemoryBlock() {};
				virtual ~VulkanDeviceMemoryBlock() = 0 {};

				//Override shutdown
				void Shutdown() override;

				//Override init
				bool Init(EngineAPI::Graphics::DeviceMemoryStore* parentStore, EUINT_64 memoryBlockSize,
					EUINT_64 memoryBlockOffset, bool isBlockMappable) override;

			protected:
				//Vulkan handle of the device memory this block resides in
				VkDeviceMemory parentVKDeviceMemory = VK_NULL_HANDLE;
			};
		};
	};
};