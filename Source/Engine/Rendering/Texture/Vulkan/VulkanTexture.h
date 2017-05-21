//VulkanTexture.h
//Created 29/04/17
//Created By Daniel Bowler
//
//Represents a vulkan texture - base class for specific texture types (eg: Texture2D objects, 
//render targets, depth buffers etc)

#pragma once

//Base class
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
			class VulkanTexture : public EngineAPI::Rendering::Resource
			{
			public:
				VulkanTexture(RenderingResourceType type) : Resource::Resource(type) {};
				virtual ~VulkanTexture() = 0 {};

				//Shutdown the texture object - called manually by the subclasses. 
				virtual void Shutdown() = 0;

			public:
				//Getters
				VkImage GetVKImageHandle() { return vkImageHandle; };

				//Image settings
				VkExtent3D GetVkImageDimentions()		        { return vkTextureDimentions; };
				VkFormat GetVkImageFormat()					    { return vkTextureFormat; };
				VkImageType GetVKImageType()				    { return vkImageType; };
				VkImageUsageFlags GetVKImageUsageFlags()	    { return vkImageUsageFlags; };
				VkSampleCountFlags GetVKImageSamplesCountFlag() { return vkSamplesCountFlag; };
				VkImageTiling GetVKImageTilingMode()			{ return vkImageTilingMode; };

			protected:
				//Inits the Vulkan texture (aka, VkImage) - used by subclass.
				//
				//NOTE: Will also cache the memory requirements for this resource. 
				bool InitVKTexture(EngineAPI::Graphics::RenderDevice* renderingDevice, 
					VkImageCreateInfo* imageCreateInfo, bool isTextureDynamic);

				//Allocs the texture on the device (or host if thats what we actually want)
				bool AllocAndBindVKTextureMemory(EngineAPI::Graphics::RenderDevice* renderingDevice,
					EngineAPI::Graphics::DeviceMemoryStore* optionalMemoryStore = nullptr);

			protected:
				//Vulkan texture / image resource handle
				VkImage vkImageHandle = VK_NULL_HANDLE;

				//Cached logical device that owns the resource
				VkDevice cachedVkDevice = VK_NULL_HANDLE;

			protected:
				//Cached texture info
				//
				//Dimensions
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
