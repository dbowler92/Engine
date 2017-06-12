//VulkanStagingBuffer.h
//Created 12/06/17
//Created By Daniel Bowler
//
//Vulkans way of transferring image and buffer data from host memory to
//device memory

#pragma once

//Base class
#include "../../../Rendering/Buffer/Buffer.h"

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Device used to create this...
#include "../../../Graphics/Device/RenderDevice.h"

//Vulkan header
#include <vulkan/vulkan.h>

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanStagingBuffer : public EngineAPI::Rendering::Buffer
			{
			public:
				VulkanStagingBuffer(RenderingResourceType type) : Buffer(type) {};
				virtual ~VulkanStagingBuffer() = 0 {};

				//Shutdown the staging buffer
				void Shutdown();

				//1) Init the staging buffer
				bool InitVKStagingBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
					VkDeviceSize stagingBufferSizeBytes);

				//2) Alloc the host visible staging buffer
				bool AllocAndBindHostVisibleVKStagingBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
					void* stagingData, EngineAPI::Graphics::DeviceMemoryStore* optionalDeviceStore = nullptr);
			};
		};
	};
};