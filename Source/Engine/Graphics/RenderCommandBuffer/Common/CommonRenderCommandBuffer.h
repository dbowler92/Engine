//CommonRenderCommandBuffer.h
//Created 21/04/17
//Created By Daniel Bowler
//
//Interface class that platform specific rendering command buffers
//will inherit from

#pragma once

//Globals
#include "../../../Main/EngineStd.h"

//Debug
#include "../../../Debug/Log/DebugLog.h"

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Interface
		{
			class CommonRenderCommandBuffer
			{
				//TODO: The CommandBufferPool class will be responsable for creating
				//command buffer objects. 
				//friend class CommonCommandBufferPool;

			public:
				CommonRenderCommandBuffer() {};
				virtual ~CommonRenderCommandBuffer() = 0 {};

				//Override the shutdown functions
				virtual void Shutdown() = 0;

				//Override the reset function - doesnt destroy/free the buffer by default (release memory back
				//to the command pool used to create this buffer)
				virtual bool ResetCommandBuffer(bool shouldReleaseMemoryToCommandPool = false) = 0;

			public:
				//Override the begin/end reading function - We will be recording rendering
				//commands in to this cmd buffer
				virtual bool BeginRecordingToCommandBuffer() = 0;
				virtual bool EndRecordingToCommandBuffer() = 0;
			};
		};
	};
};
