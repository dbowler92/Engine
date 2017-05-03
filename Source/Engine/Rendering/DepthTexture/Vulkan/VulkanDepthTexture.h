//VulkanDepthTexture.h
//Created 03/05/17
//Created By Daniel Bowler
//
//Vulkan implementation for a depth buffer (image)

#pragma once

//Inc base
#include "../Common/CommonDepthTexture.h"

namespace EngineAPI
{
	namespace Rendering
	{
		namespace Platform
		{
			class VulkanDepthTexture : public EngineAPI::Rendering::Interface::CommonDepthTexture
			{
			public:
				VulkanDepthTexture() {};
				virtual ~VulkanDepthTexture() = 0 {};

				//Override init and shutdown with Vulkan specific code. 
				bool Init(EngineAPI::Graphics::RenderDevice* renderingDevice,
					DepthTextureFormat depthTextureFormat, ESize2D depthTextureDimentions,
					DepthTextureUsageFlags depthTextureUsageFlags) override;
				void Shutdown() override;
			};
		};
	};
};
