//VulkanTextureResource.h
//Created 29/04/17
//Created By Daniel Bowler
//
//Represents a VkImage

#pragma once

//Base texture resource
#include "../Common/CommomTextureResource.h"

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
			};
		};
	};
};
