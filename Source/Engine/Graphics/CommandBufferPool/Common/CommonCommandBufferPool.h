//CommonCommandBufferPool.h
//Created 24/04/17
//Created by Daniel Bowler
//
//Interface that our API specific (render) command buffer pool
//will inherit from. The device will be responsable for managing
//these pools (should be able to support multiple pools)

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
			class CommonCommandBufferPool
			{
				//TODO: The rendering device should be the one who creates and manages
				//command pools. 
				//friend class RenderDevice;

			public:
				CommonCommandBufferPool() {};
				virtual ~CommonCommandBufferPool() = 0 {};

				//Override the shutdown functions
				virtual void Shutdown() = 0;

				//Override the reset pool function
				virtual bool ResetCommandBufferPool(bool doReleaseCommandBuffersBackToPool = false) = 0;
			};
		};
	};
};
