//VulkanSampler2D.h
//Created 08/06/17
//Created By Daniel Bowler
//
//Represents a Vulkan image that can be sampled in a shader. Eg: Diffuse texture, normal map etc.

#pragma once

//Base class
#include "../../Texture/Texture.h"

//Does need to know about the device to create it
#include "../../../Graphics/Device/RenderDevice.h"

namespace EngineAPI
{
	namespace Rendering
	{
		namespace Platform
		{
			class VulkanSampler2D : public EngineAPI::Rendering::Texture
			{
			public:
				VulkanSampler2D(RenderingResourceType type) : Texture(type) {};
				virtual ~VulkanSampler2D() = 0 {};

				//Shutsdown the sampler2D. 
				void Shutdown();
			};
		};
	};
};