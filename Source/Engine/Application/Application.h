//Application.h
//Created 23/07/16
//Created by Daniel Bowler
//
//Application layer of the engine. Each game that uses this engine
//should define a game specific class that inherits from this
//base application class & implement the virtual functions.

#pragma once

#include "../Config/EngineConfig.h" //Engine settings
//#include "../Main/EngineStd.h"

#ifdef ENGINE_CONFIG_PLATFORM_WIN32
#include "Win32\Win32Application.h"
#endif 

namespace EngineAPI 
{
	namespace Core
	{
		class Application : public OS_PLATFORM_IMPLEMENTATION(Application)
		{
			HIDE_COPY_ASSIGNMENT(Application)
		public:
			Application() {};
			virtual ~Application() = 0 {};

			//
			//Virtual functions. Game specific instance of this class should implement these
			//
			virtual ECHAR* GetGameTitle() = 0;
			
			//Inits your game - called after engine and subsystems have been 
			//inited. Here, you would create your game specific subsystems and
			//assets etc. 
			virtual bool InitApplication() = 0;

			//Called when the engine is shutdown before closing the actual engine. 
			virtual bool ShutdownApplication() = 0;
		};
	};
};