//DebugLog.h
//Created 16/04/17
//Cretaed By Daniel Bowler
//
//Helper class which will be used to output debug info to the console, output window
//or log file (added as and when needed) - should be platform independant

#pragma once

#include "../../Main/EngineStd.h"

namespace EngineAPI
{
	namespace Debug
	{
		class DebugLog
		{
		public:
			//To VS output window
			static void PrintInfoMessage(const ECHAR* msg);
			static void PrintWarningMessage(const ECHAR* msg);
			static void PrintErrorMessage(const ECHAR* msg);

		};
	};
};
