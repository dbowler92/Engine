//OSWindow.h
//Created 19/04/17
//Created By Daniel Bowler
//
//Represents the OS created window that our graphics API will talk too.

#pragma once

//Platform
#include "../Config/EngineConfig.h"

//Which os to inherit from
#ifdef ENGINE_CONFIG_PLATFORM_WIN32
#include "Win32\Win32OSWindow.h"
#endif

namespace EngineAPI
{
	namespace OS
	{
		class OSWindow : public OS_PLATFORM_IMPLEMENTATION(OSWindow)
		{
		public:
			OSWindow() {};
			~OSWindow() {};
			
		};
	};
};