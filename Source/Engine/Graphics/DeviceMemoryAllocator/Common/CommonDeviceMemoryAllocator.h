//CommonDeviceMemoryAllocator.h
//Created 04/05/17
//Created By Daniel Bowler
//
//Common interface that each device API specific memory manager will inherit from

#pragma once

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Interface
		{
			class CommonDeviceMemoryAllocator
			{
			public:
				CommonDeviceMemoryAllocator() {};
				virtual ~CommonDeviceMemoryAllocator() = 0 {};

				//Override shutdown function
				virtual void Shutdown() = 0;
			};
		}
	}
}