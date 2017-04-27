//CommonCommandQueue.h
//Created 24/04/17
//Created By Daniel Bowler
//
//Base class that platform specifc abstractions will inherit from

#pragma once

//Take rendering commands (buffers) and sends them to the GPU
#include "../../RenderCommandBuffer/RenderCommandBuffer.h"

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Interface
		{
			class CommonCommandQueue
			{
			public:
				CommonCommandQueue() {};
				virtual ~CommonCommandQueue() = 0 {};

				//Override shutdown function for cleanup
				virtual void Shutdown() = 0;

			public:
				//Override: Queue submission function. Takes a cmd buffer and submits
				//the work to this queue
				virtual bool SubmitCommandBuffer(EngineAPI::Graphics::RenderCommandBuffer* cmdBuffer) = 0;
				virtual bool SubmitCommandBufferAndWait(EngineAPI::Graphics::RenderCommandBuffer* cmdBuffer) = 0;

				//Takes an array of command buffers and submits the work to this queue
				virtual bool SubmitCommandBuffersArray(EngineAPI::Graphics::RenderCommandBuffer* cmdBuffers, uint32_t cmdBuffersCount) = 0;
				virtual bool SubmitCommandBuffersArrayAndWait(EngineAPI::Graphics::RenderCommandBuffer* cmdBuffers, uint32_t cmdBuffersCount) = 0;
			};
		};
	};
};
