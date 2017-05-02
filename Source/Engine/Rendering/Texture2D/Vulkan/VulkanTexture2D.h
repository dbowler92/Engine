//VulkanTexture2D.h
//Created 29/04/17
//Created By Daniel Bowler
//
//Represents a vulkan texture2D

#pragma once

//Base class
#include "../Common/CommonTexture2D.h"

//Vulkan header
#include <vulkan/vulkan.h>

namespace EngineAPI
{
	namespace Rendering
	{
		namespace Platform
		{
			class VulkanTexture2D : public EngineAPI::Rendering::Interface::CommonTexture2D
			{
			public:
				VulkanTexture2D() {};
				virtual ~VulkanTexture2D() = 0 {};

				//Shutdown the texture2D object
				void Shutdown() override;

			public:
				//Inits the Vulkan texture2D (aka, VkImage) - TODO: Replace with a less platform specific Init function!
				bool InitVKTexture2D(EngineAPI::Graphics::RenderDevice* renderingDevice, VkImageCreateInfo* imageCreateInfo);

			protected:
				//Vulkan texture / image resource handle
				VkImage vkImageHandle = VK_NULL_HANDLE;

				//Cached logical device that owns the resource
				VkDevice cachedVkDevice = VK_NULL_HANDLE;
			};
		}
	};
};
