//DebugLog.h
//Created 16/04/17
//Created By Daniel Bowler
//
//Helper class which will be used to output debug info to the console, output window
//or log file (added as and when needed) - should be platform independent

#pragma once

#include "../../Main/EngineStd.h"
#include "../../Config/EngineConfig.h"

//Assert if required
#include <assert.h>

namespace EngineAPI
{
	namespace Debug
	{
		class DebugLog
		{
		public:
			//To VS output window
			static void PrintMessage(const ECHAR* msg);
			static void PrintInfoMessage(const ECHAR* msg);
			static void PrintWarningMessage(const ECHAR* msg);
			static void PrintErrorMessage(const ECHAR* msg);
		};
	};
};
