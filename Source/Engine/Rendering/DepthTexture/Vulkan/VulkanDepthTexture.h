//VulkanDepthTexture.h
//Created 03/05/17
//Created By Daniel Bowler
//
//Vulkan implementation for a depth buffer (image)

#pragma once

//Does need to know about the device to create it
#include "../../../Graphics/Device/RenderDevice.h"

//Uses Texture2D to create the actual texture resource
#include "../../Texture2D/Texture2D.h"

//Depth formats
enum DepthTextureFormat
{
	DEPTH_TEXTURE_FORMAT_D16,
	//DEPTH_TEXTURE_FORMAT_D24,
	DEPTH_TEXTURE_FORMAT_D32,
	DEPTH_TEXTURE_FORMAT_D24_S8
};

//Depth texture usage
typedef enum DepthTextureUsageFlagsBits
{
	DEPTH_TEXTURE_USAGE_SHADER_INPUT_BIT = 0x0000001
} DepthTextureUsageFlagsBits;
typedef EFlag DepthTextureUsageFlag;

namespace EngineAPI
{
	namespace Rendering
	{
		namespace Platform
		{
			class VulkanDepthTexture
			{
			public:
				VulkanDepthTexture() {};
				virtual ~VulkanDepthTexture() = 0 {};

				//Init and shutdown
				bool InitVKDepthTexture(EngineAPI::Graphics::RenderDevice* renderingDevice,
					DepthTextureFormat depthTextureFormat, ESize2D depthTextureDimentions,
					DepthTextureUsageFlag depthTextureUsageFlags);
				void Shutdown();

			protected:
				//Vulkan tiling mode
				VkImageTiling vkImageTilingMode;

			protected:
				//Actual 2D texture for the depth buffer. 
				EngineAPI::Rendering::Texture2D depthTextureObject;

				//State of the depth texture
				DepthTextureFormat depthTextureFormat;
				DepthTextureUsageFlag depthTextureUsage;
			};
		};
	};
};
