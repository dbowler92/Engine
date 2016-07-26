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

		public:
			//
			//Win32
			//
			//Init app. Will bring up a W32 window - Optionally, can provide additional init code. 
			virtual bool Init(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd = NULL,
				int screenWidth = 960, int screenHeight = 540);

		public:
			//Once all is loaded, enter the main game loop
			void EnterGameLoop();

			//Shutdown the application
			void Shutdown();

		protected:
			HINSTANCE hInst;			  //App instance handle
			unsigned appWidth, appHeight; //Dimentions of application window
		};
	};
};

//Define a global pointer to the singleton app instance. Override Application
//and provide a project specific version
extern EngineAPI::Base::Application* g_App;