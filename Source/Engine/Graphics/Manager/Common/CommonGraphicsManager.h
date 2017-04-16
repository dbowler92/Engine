//CommonGraphicsManager.h
//Created 16/04/17
//Created By Daniel Bowler
//
//Inteface that each platform/API specific graphics manager should implement

#pragma once

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
				virtual bool InitSubsystem() = 0;

				//Shutsdown the graphics manager
				virtual bool ShutdownSubsystem() = 0;
			};
		};
	};
};