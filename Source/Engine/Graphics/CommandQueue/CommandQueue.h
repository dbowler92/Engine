//CommandQueue.h
//Created 24/04/17
//Created By Daniel Bowler
//
//Represents a queue that render commands (buffers) will be sent to
//which in turn will be sent to the GPU for processing. 

#pragma once


//Platform info
#include "../../Config/EngineConfig.h"

//API being used?
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanCommandQueue.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class CommandQueue : public RENDERING_PLATFORM_IMPLEMENTATION(CommandQueue)
		{
		public:
			CommandQueue() {};
			~CommandQueue() {};
		};
	};
};