//Application.h
//Created 23/07/16
//Created by Daniel Bowler
//
//Application layer of the engine. Each game that uses this engine
//should define a game specific class that inherits from this
//base application class & implement the virtual functions.

#pragma once

#include <Windows.h>
#include "../Main/EngineStd.h"
#include "../Utils/GameTimer/GameTimer.h"

namespace EngineAPI 
{
	namespace Base
	{
		class Application
		{
			HIDE_COPY_ASSIGNMENT(Application)
		public:
			Application();

			//
			//Virtual functions. Game specific instance of this class should implement these
			//
			virtual TCHAR *VGetGameTitle() = 0;
			virtual TCHAR *VGetGameAppDirectory() = 0;
			virtual HICON VGetIcon() = 0;

			//Init app. Will bring up a W32 window - Optionally, can provide additional init code. 
			virtual bool Init(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd = NULL,
				int screenWidth = 960, int screenHeight = 540);

			//Game loop and shutdown
			void EnterGameLoop();
			void Shutdown();
		
			//The global messaging function will forward messages to this function
			LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		private:
			//Internal W32 & D3D init
			void InitWin32App();
			void InitD3D11();

			//Called when the window is resized
			void OnResize();

			//Outputs FPS
			void CalculateFrameRateStats();
			
		protected:
			HINSTANCE hInst;			  //App instance handle
			HWND mainWnd;				  //Window handle
			unsigned appWidth, appHeight; //Dimentions of application window				
			bool appPaused ,minimized, maximized, resizing; //State of app
			Utils::GameTimer mainGameLoopTimer; //Calculates frame delta
		};
	};
};

//Define a global pointer to the singleton app instance. Override Application
//and provide a project specific version
extern EngineAPI::Base::Application* g_App;