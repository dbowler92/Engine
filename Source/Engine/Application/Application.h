//Application.h
//Created 23/07/16
//Created by Daniel Bowler
//
//Application layer of the engine. Each game that uses this engine
//should define a game specific class that inherits from this
//base application class & implement the virtual functions.
//
//This is for Win32 applications - Define a custom engine level base application
//class per platform if desired. Use the precompiler to decide which Application
//should be used.  

#pragma once

#include <Windows.h>
#include <iostream>
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
			~Application();

			//
			//Virtual functions. Game specific instance of this class should implement these
			//
			virtual TCHAR* GetApplicationTitle() = 0;
			virtual TCHAR* GetApplicationDirectory() = 0;
			virtual HICON  GetIcon() = 0;

			//Called once the engine has been inited. Here, you will init the game
			//you are creating. Eg: Loading the first scene etc
			virtual bool InitApplication() = 0;

		public:
			//The global messaging function will forward messages to this function
			LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

			//Init Engine.  
			bool InitEngine(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd = NULL,
				int screenWidth = 960, int screenHeight = 540);

		public:
			//Game loop and shutdown
			void EnterGameLoop();
			void ShutdownEngine();

		protected:
			HINSTANCE hInst;			  //App instance handle
			HWND mainWnd;				  //Window handle
			unsigned appWidth, appHeight; //Dimentions of application window				
			bool appPaused, minimized, maximized, resizing; //State of app
			Utils::GameTimer mainGameLoopTimer; //Calculates frame delta

		protected:
			//Helper function which prints the name of the application to the
			//console
			void PrintApplicationTitleToConsole();

		private:
			//Creates a W32 window. 
			void InitWindow();

			//Init engine subsystems
			void InitSubsystems();
			
			//Called when the window is resized
			void OnResize();

			//Outputs FPS
			void CalculateFrameRateStats();
		};
	};
};

//Define a global pointer to the singleton app instance. Override Application
//and provide a project specific version
extern EngineAPI::Base::Application* g_App;