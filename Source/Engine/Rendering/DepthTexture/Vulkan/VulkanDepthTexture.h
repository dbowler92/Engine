//VulkanDepthTexture.h
//Created 03/05/17
//Created By Daniel Bowler
//
//Vulkan implementation for a depth buffer (image)

#pragma once

//Base class
#include "../../Texture/Texture.h"

//Does need to know about the device to create it
#include "../../../Graphics/Device/RenderDevice.h"

//Depth formats
enum DepthTextureFormat
{
	DEPTH_TEXTURE_FORMAT_D16,
	DEPTH_TEXTURE_FORMAT_D32,
	DEPTH_TEXTURE_FORMAT_D16_S8,
	DEPTH_TEXTURE_FORMAT_D24_S8,
	DEPTH_TEXTURE_FORMAT_D32_S8
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
			class VulkanDepthTexture : public EngineAPI::Rendering::Texture
			{
			public:
				VulkanDepthTexture(RenderingResourceType type) : Texture(type) {};
				virtual ~VulkanDepthTexture() = 0 {};

				//Shutsdown the depth texture. 
				void Shutdown();

				//Init
				bool InitVKDepthTexture(EngineAPI::Graphics::RenderDevice* renderingDevice,
					DepthTextureFormat depthTextureFormat, ESize2D depthTextureDimentions,
					DepthTextureUsageFlag depthTextureUsageFlags);

				//Once the depth texture has been inited, we can allocate memory for it
				bool AllocAndBindVKDepthTexture(EngineAPI::Graphics::RenderDevice* renderingDevice,
					EngineAPI::Graphics::DeviceMemoryStore* optionalDeviceStore = nullptr);

				//Inits the depth texture views and layout. 
				bool InitVKDepthTextureLayoutAndViews(EngineAPI::Graphics::RenderDevice* renderingDevice);
				
			protected:
				//State of the depth texture
				DepthTextureFormat depthTextureFormat;
				DepthTextureUsageFlag depthTextureUsage;

				//Contains stencil buffer?
				bool doesContainStencilComponent = false;

			protected:
				//Depth (stencil) image view
				VkImageView vkDepthStencilView = VK_NULL_HANDLE;

				//
				//TODO: Shader input
				//

				//Command buffers
				VkCommandBuffer vkDepthTextureImageLayoutCmdBuffer = VK_NULL_HANDLE;

			protected:
				//Internal init
				bool InitVKDepthTextureLayout(EngineAPI::Graphics::RenderDevice* renderingDevice);
				bool InitVKDepthTextureViews(EngineAPI::Graphics::RenderDevice* renderingDevice);
			};
		};
	};
};
