//Win32OSWindow.h
//Created 19/04/17
//Created By Daniel Bowler
//
//Represents an Win32 OS window

#pragma once

//Base
#include "../Common/CommonOSWindow.h"

//Windows
#include <Windows.h>

namespace EngineAPI
{
	namespace OS
	{
		namespace Platform
		{
			class Win32OSWindow : public EngineAPI::OS::Interface::CommonOSWindow
			{
			public:
				Win32OSWindow() {};
				virtual ~Win32OSWindow() = 0 {};

				//Init function
				void SetWin32AppInstance(HINSTANCE handle) { appInstanceHandle = handle; };
				void SetWin32AppMainWindowHandle(HWND handle) { appMainWindowHandle = handle; };

				//Returns Win32 data - TODO: Really want a more generic way of doing this tbh...
				HINSTANCE GetAppInstanceHandle()   { return appInstanceHandle; };
				HWND      GetAppMainWindowHandle() { return appMainWindowHandle; };

			private:
				//Win32 assigned handles
				HINSTANCE appInstanceHandle;							    //App instance handle
				HWND appMainWindowHandle;								    //Window handle	
			};
		};
	};
};