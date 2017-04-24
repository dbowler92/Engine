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

//Need to know about the device(and maybe instance)
#include "../../Device/RenderDevice.h"
#include "../../Instance/RenderInstance.h"

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Interface
		{
			class CommonRenderCommandBuffer
			{
			public:
				CommonRenderCommandBuffer() {};
				virtual ~CommonRenderCommandBuffer() = 0 {};

				//Override the init and shutdown functions
				virtual bool Init(EngineAPI::Graphics::RenderDevice* renderDevice, 
					EngineAPI::Graphics::RenderInstance* renderInstance) = 0;
				virtual void Shutdown() = 0;

				//Override the begin/end reading function - We will be recording rendering
				//commands in to this cmd buffer
				virtual bool BeginRecordingToCommandBuffer() = 0;
				virtual bool EndRecordingToCommandBuffer() = 0;
			};
		};
	};
};
