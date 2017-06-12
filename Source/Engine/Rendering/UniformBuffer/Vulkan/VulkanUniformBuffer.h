//VulkanUniformBuffer.h
//Created 05/07/17
//Created By Daniel Bowler
//
//(AKA Constant buffers) - Buffer objects that can be fed in to and ready within a shader 

#pragma once

//Base class
#include "../../Buffer/Buffer.h"

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Device
#include "../../../Graphics/Device/RenderDevice.h"

//Vulkan header
#include <vulkan/vulkan.h>

namespace EngineAPI
{
	namespace Rendering
	{
		namespace Platform
		{
			class VulkanUniformBuffer : public EngineAPI::Rendering::Buffer
			{
			public:
				VulkanUniformBuffer(RenderingResourceType type) : Buffer(type) {};
				virtual ~VulkanUniformBuffer() = 0 {};

				//Shutdown the descriptor object
				void Shutdown();

				//1) Inits the uniform buffer
				bool InitVKUniformBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
					VkDeviceSize uniformBufferSizeBytes, RenderingResourceUsage resourceUsage);

				//2) Allocs the uniform buffer on the device -> This includes writing the
				//data to the buffer & binding device memory
				bool AllocAndBindVKUniformBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
					void* uniformBufferData, EngineAPI::Graphics::DeviceMemoryStore* optionalDeviceStore = nullptr);

				//3) (Optional) Create the buffer views. 
				bool InitVKUniformViews(EngineAPI::Graphics::RenderDevice* renderingDevice);
			};
		};
	};
};