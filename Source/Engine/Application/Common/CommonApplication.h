//CommonApplication.h
//Created 15/14/17
//Created By Daniel Bowler
//
//Interface class which all platform specific application classes should
//implement. 

#pragma once

#include "../../Config/EngineConfig.h" //Engine settings
#include "../../Main/EngineStd.h"
#include "../../Debug/Log/DebugLog.h" //Debug logging

namespace EngineAPI
{
	namespace Core
	{
		namespace Interface
		{
			class CommonApplication
			{
			public:
				CommonApplication() {};

				//Define in your own game applications
				//
				//Game title
				virtual ECHAR* GetGameTitle() = 0;

				//Inits your game - called after engine and subsystems have been 
				//inited. Here, you would create your game specific subsystems and
				//assets etc. 
				virtual bool InitApplication() = 0;

				//Called when the engine is shutdown before closing the actual engine. 
				virtual bool ShutdownApplication() = 0;

			public:
				//Called internally by the engine
				//
				//virtual bool InitEngine(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd = NULL,
				//	int screenWidth = 960, int screenHeight = 540) = 0;

				//Shutsdown the engine - called after shutting down the application
				virtual bool ShutdownEngine() = 0;

				//Game loop
				virtual void EnterGameLoop() = 0; 
			};
		};
	};
};