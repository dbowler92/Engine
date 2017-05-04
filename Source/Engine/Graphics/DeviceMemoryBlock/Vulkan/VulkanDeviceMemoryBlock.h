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
				virtual void Shutdown() override;
			};
		};
	};
};