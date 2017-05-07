//Resource.h
//Created 07/05/17
//Created By Daniel Bowler
//
//Represents a graphics/rendering (/Compute) resource

#pragma once

//Platform
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanResource.h"
#endif

namespace EngineAPI
{
	namespace Rendering
	{
		class Resource : public RENDERING_PLATFORM_IMPLEMENTATION(Resource)
		{
		public:
			Resource(RenderingResourceType type) : RENDERING_PLATFORM_CLASS(Resource)(type) {};
			virtual ~Resource() = 0 {};

			//Each resource will have a shutdown function to implement
			virtual void Shutdown() = 0;
		};
	};
};
