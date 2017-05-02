//VulkanTextureResource.h
//Created 29/04/17
//Created By Daniel Bowler
//
//Represents a VkImage

#pragma once

//Base texture resource
#include "../Common/CommomTextureResource.h"

//Vulkan header
#include <vulkan/vulkan.h>

namespace EngineAPI
{
	namespace Rendering
	{
		namespace Platform
		{
			class VulkanTextureResource : public EngineAPI::Rendering::Interface::CommonTextureResource
			{
			public:
				VulkanTextureResource() {};
				virtual ~VulkanTextureResource() = 0 {};

				//Override the shutdown - deletes the VkImage
				virtual void Shutdown() override; //Texture2D etc will override this with its own shutdown code + remember to call super version!

			protected:
				//Inits the Vulkan image resource. This should be called
				//by the subclass
				bool InitVKTexture(VkDevice* logicalDevice, VkImageCreateInfo* imageCreateInfo);

			protected:
				//Vulkan texture / image resource handle
				VkImage vkImageHandle = VK_NULL_HANDLE;

				//Cached logical device that owns the resource
				VkDevice cachedVkDevice = VK_NULL_HANDLE;
			};
		};
	};
};
