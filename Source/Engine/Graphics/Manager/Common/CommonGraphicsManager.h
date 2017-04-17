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

				//Inits the graphics subsystem / manager
				virtual bool InitSubsystem(ECHAR* appTitle,
					int appVersionMajor, int appVersionMinor, int appVersionPatch,
					unsigned screenWidth, unsigned screenHeight) = 0;

				//Shutsdown the graphics manager
				virtual bool ShutdownSubsystem() = 0;
			};
		};
	};
};