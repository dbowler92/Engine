//CommonRenderInstance.h
//Created 20/04/17
//Created By Daniel Bowler
//
//All platform / API specific rendering instances will inherit from this base
//class


#pragma once

//Globals
#include "../../../Main/EngineStd.h"

//Debug
#include "../../../Debug/Log/DebugLog.h"

//May need to know about the OS window
#include "../../../OSWindow/OSWindow.h"

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Interface
		{
			class CommonRenderInstance
			{
			public:
				CommonRenderInstance() {};
				virtual ~CommonRenderInstance() = 0 {};

				//Override shutdown function
				virtual void Shutdown() = 0 {};
			};
		}
	};
};