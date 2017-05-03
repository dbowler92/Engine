//CommonDepthTexture.h
//Created 03/05/17
//Created By Daniel Bowler
//
//Common interface for depth buffers / textures that each API
//will provide an implementation for

#pragma once

//Does need to know about the device to create it
#include "../../../Graphics/Device/RenderDevice.h"

//Depth formats
enum DepthTextureFormat
{
	DEPTH_TEXTURE_FORMAT_D16, 
	DEPTH_TEXTURE_FORMAT_D24,
	DEPTH_TEXTURE_FORMAT_D32,
	DEPTH_TEXTURE_FORMAT_D24_S8
};

//Depth texture usage
enum DepthTextureUsageFlags
{
	DEPTH_TEXTURE_USAGE_RENDER_TARGET = 0x0000001,
	DEPTH_TEXTURE_USAGE_SHADER_INPUT  = 0x0000002
};

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
					DepthTextureUsageFlags depthTextureUsageFlags) = 0 {};
				virtual void Shutdown() = 0 {};
			};
		};
	};
};