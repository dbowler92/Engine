//Buffer.h
//Created 18/05/17
//Created By Daniel Bowler
//
//Represents a rendering buffer object

#pragma once

//Platform
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanBuffer.h"
#endif

namespace EngineAPI
{
	namespace Rendering
	{
		class Buffer : public RENDERING_PLATFORM_IMPLEMENTATION(Buffer)
		{
		public:
			Buffer(RenderingResourceType type) : RENDERING_PLATFORM_CLASS(Buffer)(type) {};
			virtual ~Buffer() = 0 {};

			//Shutdown texture
			virtual void Shutdown() { __super::Shutdown(); };
		};
	};
};