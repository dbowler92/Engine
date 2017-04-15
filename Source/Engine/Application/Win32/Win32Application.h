//Win32Application.h
//Created 15/14/17
//Created By Daniel Bowler
//
//Win32 implementation of the base application class

#pragma once

#include <Windows.h>
#include "../Common/CommonApplication.h"
#include "../../Main/EngineStd.h"
#include "../../Utils/GameTimer/GameTimer.h"

namespace EngineAPI
{
	namespace Core
	{
		class Win32Application : public CommonApplication
		{
			HIDE_COPY_ASSIGNMENT(Win32Application)
		public:
			Win32Application();

			//
			//Virtual functions. Game specific instance of this class should implement these
			//
			virtual char *GetGameTitle() = 0;

			//Inits your game - called after engine and subsystems have been 
			//inited. Here, you would create your game specific subsystems and
			//assets etc. 
			virtual bool InitApplication() = 0;

			//Called when the engine is shutdown before closing the actual engine. 
			virtual bool ShutdownApplication() = 0;

		public:
			//Engine
			//
			//The global messaging function will forward messages to this function
			LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

			//Init engine & engine subsystems. Will bring up a W32 window. Called before initing the 
			//game 
			bool InitEngine(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd = NULL,
				int screenWidth = 960, int screenHeight = 540);

			//Shutsdown the engine - called after shutting down the 
			bool ShutdownEngine();

			//Game loop
			void EnterGameLoop();

		private:
			//Inits Win32 application
			bool InitWin32App();

			//Inits the engine subsystems - eg: Graphics, physics, etc
			bool InitEngineSubsystems();

			//Called when the window is resized
			void OnResize();

		private:
			//Outputs FPS
			void CalculateFrameRateStats();

		protected:
			HINSTANCE hInst;			  //App instance handle
			HWND mainWnd;				  //Window handle
			unsigned appWidth, appHeight; //Dimentions of application window				
			bool appPaused, minimized, maximized, resizing; //State of app
			Utils::GameTimer mainGameLoopTimer; //Calculates frame delta
		};
	};
};