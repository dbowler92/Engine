//VulkanTexture2D.h
//Created 29/04/17
//Created By Daniel Bowler
//
//Represents a vulkan texture2D

#pragma once

//Base class
#include "../Common/CommonTexture2D.h"

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

			};
		}
	};
};
