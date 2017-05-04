//CommonDeviceMemoryBlock.h
//Created 04/05/17
//Created By Daniel Bowler
//
//Common interface that API specific device memory blocks will inherit from

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
			class CommonDeviceMemoryBlock
			{
			public:
				CommonDeviceMemoryBlock() {};
				virtual ~CommonDeviceMemoryBlock() = 0 {};

				//Override the shutdown function
				virtual void Shutdown() = 0;
			};
		};
	};
};

