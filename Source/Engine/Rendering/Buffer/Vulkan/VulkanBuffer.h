//VulkanBuffer.h
//Created 18/05/17
//Created By Daniel Bowler
//
//Represents a Vulkan buffer resource

#pragma once

//Is a resource
#include "../../Resource/Resource.h"

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Device used to create this...
#include "../../../Graphics/Device/RenderDevice.h"

//Stores
#include "../../../Graphics/DeviceMemoryStore/DeviceMemoryStore.h"

//Vulkan header
#include <vulkan/vulkan.h>

namespace EngineAPI
{
	namespace Rendering
	{
		namespace Platform
		{
			class VulkanBuffer : public EngineAPI::Rendering::Resource
			{
			public:
				VulkanBuffer(RenderingResourceType type) : Resource(type) {};
				virtual ~VulkanBuffer() = 0 {};

				//Shutdown the buffer object - called manually by the subclasses. 
				virtual void Shutdown() = 0;

			public:
				//Returns the VKBuffer handle
				VkBuffer GetVKBufferHandle() { return vkBufferHandle; };

				//Returns the buffer descriptor info
				VkDescriptorBufferInfo& GetVKDescriptorBufferInfo() { return vkBufferDescriptorInfo; };

				//General getters. 
				VkDeviceSize GetBufferContentsSize() { return bufferContentsSizeBytes; };

			protected:
				//Inits the buffer
				bool InitVKBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
					VkBufferCreateInfo* bufferCreateInfo, bool isDynamicBuffer, RenderingResourceUsage resourceUsage);

				//Allocates a DeviceMemoryBlock for this resource
				bool AllocVKBufferMemoryBlock(EngineAPI::Graphics::RenderDevice* renderingDevice,
					EngineAPI::Graphics::DeviceMemoryStore* optionalMemoryStore = nullptr);

				//Staging - writes the data to the buffer
				bool WriteDataToVKBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice, void* data);

				//Bind device memory
				bool BindVKBufferMemory(EngineAPI::Graphics::RenderDevice* renderingDevice);

			protected:
				//Cache the device used to create the buffer
				VkDevice cachedVKLogicalDevice = VK_NULL_HANDLE;

			protected:
				//Vulkan buffer handle
				VkBuffer vkBufferHandle = VK_NULL_HANDLE;

				//Buffer description
				VkDescriptorBufferInfo vkBufferDescriptorInfo;

			protected:
				//Buffer size (bytes)
				VkDeviceSize bufferContentsSizeBytes = 0;
			};
		};
	};
};