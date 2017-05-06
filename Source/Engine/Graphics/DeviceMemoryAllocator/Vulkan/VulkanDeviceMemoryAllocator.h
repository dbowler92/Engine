//VulkanDeviceMemoryAllocator.h
//Created 04/05/17
//Created By Daniel Bowler
//
//Handles memory management of GPU resources for Vulkan.

#pragma once

//Base
#include "../Common/CommonDeviceMemoryAllocator.h"

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanDeviceMemoryAllocator : public EngineAPI::Graphics::Interface::CommonDeviceMemoryAllocator
			{
			public:
				VulkanDeviceMemoryAllocator() {};
				virtual ~VulkanDeviceMemoryAllocator() = 0 {};

				//Override shutdown function
				void Shutdown() override;

				//Override init function
				bool Init() override;
			};
		}
	}
}