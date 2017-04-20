//CommonSwapchain.h
//Created 20/04/17
//Created by Daniel Bowler
//
//Common interface that will be inherited by graphics API specific
//implementations. The swapchain will created the doube render buffer
//and interact with the OS windowing system

#pragma once

//OS window, graphics instance and device maybe needed
//to setup the swapchain
#include "../../../OSWindow/OSWindow.h"
#include "../../Instance/RenderInstance.h"
#include "../../Device/RenderDevice.h"

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Interface
		{
			class CommonSwapchain
			{
			public:
				CommonSwapchain() {};
				virtual ~CommonSwapchain() = 0 {};

				//Override the init and shutdown functions
				virtual bool Init(EngineAPI::OS::OSWindow* osWindow,
					EngineAPI::Graphics::RenderInstance* renderingInstance,
					EngineAPI::Graphics::RenderDevice* renderingDevice) = 0 {};
				virtual void Shutdown() = 0 {};
			};
		}
	};
};
