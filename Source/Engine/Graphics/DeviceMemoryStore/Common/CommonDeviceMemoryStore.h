//CommonDeviceMemoryStore.h
//Created 28/04/17
//Created By Daniel Bowler
//
//Interface class that API specific implementations must implement. 

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
			class CommonDeviceMemoryStore
			{
			public:
				CommonDeviceMemoryStore() {};
				virtual ~CommonDeviceMemoryStore() = 0 {};

				//Override the shutdown function
				virtual void Shutdown() = 0;
			};
		};
	};
};