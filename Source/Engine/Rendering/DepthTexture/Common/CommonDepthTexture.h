//CommonDepthTexture.h
//Created 03/05/17
//Created By Daniel Bowler
//
//Common interface for depth buffers / textures that each API
//will provide an implementation for

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
	DEPTH_TEXTURE_USAGE_SHADER_INPUT_BIT  = 0x0000001
} DepthTextureUsageFlagsBits;
typedef EFlag DepthTextureUsageFlag;
 
namespace EngineAPI
{
	namespace Rendering
	{
		namespace Interface
		{
			class CommonDepthTexture
			{
			public:
				CommonDepthTexture() {};
				virtual ~CommonDepthTexture() = 0 {};

				//Override init and  shutdown function
				virtual bool Init(EngineAPI::Graphics::RenderDevice* renderingDevice,
					DepthTextureFormat depthTextureFormat, ESize2D depthTextureDimentions,
					DepthTextureUsageFlag depthTextureUsageFlags) = 0 {};
				virtual void Shutdown() = 0 {};

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