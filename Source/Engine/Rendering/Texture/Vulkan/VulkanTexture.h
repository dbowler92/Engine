//VulkanTexture.h
//Created 29/04/17
//Created By Daniel Bowler
//
//Represents a vulkan texture2D

#pragma once

//Base class
#include "../../Resource/Resource.h"

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Device used to create this...
#include "../../../Graphics/Device/RenderDevice.h"

//Vulkan header
#include <vulkan/vulkan.h>

namespace EngineAPI
{
	namespace Rendering
	{
		namespace Platform
		{
			class VulkanTexture : public EngineAPI::Rendering::Resource
			{
			public:
				VulkanTexture();
				virtual ~VulkanTexture() = 0 {};

				//Shutdown the texture2D object
				void Shutdown();

			public:
				//Inits the Vulkan texture (aka, VkImage)
				bool InitVKTexture(EngineAPI::Graphics::RenderDevice* renderingDevice, VkImageCreateInfo* imageCreateInfo);

			protected:
				//Vulkan texture / image resource handle
				VkImage vkImageHandle = VK_NULL_HANDLE;

				//Cached logical device that owns the resource
				VkDevice cachedVkDevice = VK_NULL_HANDLE;

			protected:
				//Cached texture info
				//
				//Dimentions
				VkExtent3D vkTextureDimentions;

				//Image format & type
				VkFormat vkTextureFormat;
				VkImageType vkImageType; //Texture2D, 1D etc
				
				//Image usage (depth stencil buffer? Shader input? etc)
				VkImageUsageFlags vkImageUsageFlags;

				//Samples info
				VkSampleCountFlags vkSamplesCountFlag;

				//Vulkan tiling mode
				VkImageTiling vkImageTilingMode;
			};
		}
	};
};
