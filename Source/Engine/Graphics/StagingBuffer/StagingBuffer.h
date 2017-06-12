//StagingBuffer.h
//Created 12/06/17
//Created By Daniel Bowler
//
//Class which will help transfer data (textures + buffers) from CPU memory to GPU memory

#pragma once

//Platform
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanStagingBuffer.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class StagingBuffer : public RENDERING_PLATFORM_IMPLEMENTATION(StagingBuffer)
		{
		public:
			StagingBuffer() : RENDERING_PLATFORM_CLASS(StagingBuffer)(RENDERING_RESOURCE_TYPE_STAGING_BUFFER) {};
			virtual ~StagingBuffer() {};

		};
	};
};