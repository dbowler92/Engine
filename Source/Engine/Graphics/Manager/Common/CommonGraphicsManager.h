//CommonGraphicsManager.h
//Created 16/04/17
//Created By Daniel Bowler
//
//Inteface that each platform/API specific graphics manager should implement

#pragma once

//Globals
#include "../../../Main/EngineStd.h"

//Debug
#include "../../../Debug/Log/DebugLog.h"

//OS window info - so the graphics API can talk to the OS windowing system.
#include "../../../OSWindow/OSWindow.h"

//Abstractions of rendering API
#include "../../Instance/RenderInstance.h"
#include "../../Device/RenderDevice.h"

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Interface
		{
			class CommonGraphicsManager
			{
			public:
				CommonGraphicsManager() {};
				virtual ~CommonGraphicsManager() = 0 {};

				//Inits the graphics subsystem / manager
				virtual bool InitSubsystem(EngineAPI::OS::OSWindow* osWindow,
					ECHAR* appTitle, int appVersionMajor, int appVersionMinor, int appVersionPatch,
					unsigned screenWidth, unsigned screenHeight) = 0;

				//Shutsdown the graphics manager
				virtual bool ShutdownSubsystem() = 0;

			protected:
				//Instance, device and swapchain handlers
				RenderInstance renderingInstance;
				RenderDevice renderingDevice;

			};
		};
	};
};