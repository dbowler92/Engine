//Win32OSWindow.h
//Created 19/04/17
//Created By Daniel Bowler
//
//Represents an Win32 OS window
//
//TODO: Move some of the code from Win32App over to here (Eg: The window creationn & 
//registration code). 

#pragma once

//Windows
#include <Windows.h>

namespace EngineAPI
{
	namespace OS
	{
		namespace Platform
		{
			class Win32OSWindow
			{
			public:
				Win32OSWindow() {};
				virtual ~Win32OSWindow() = 0 {};

				//Init functions
				void SetWin32AppInstance(HINSTANCE handle) { appInstanceHandle = handle; };
				void SetWin32AppMainWindowHandle(HWND handle) { appMainWindowHandle = handle; };

				//Setters & getters
				void UpdateWindowWidth(unsigned w) { windowWidth = w; };
				void UpdateWindowHeight(unsigned h) { windowHeight = h; };

				unsigned GetWindowWidth() { return windowWidth; };
				unsigned GetWindowHeight() { return windowHeight; };

				//Returns Win32 data 
				HINSTANCE GetAppInstanceHandle()   { return appInstanceHandle; };
				HWND      GetAppMainWindowHandle() { return appMainWindowHandle; };

			private:
				//Win32 assigned handles
				HINSTANCE appInstanceHandle;							    //App instance handle
				HWND appMainWindowHandle;								    //Window handle	
			
			protected:
				//Window size
				unsigned windowWidth;
				unsigned windowHeight;
			};
		};
	};
};