//CommonRenderDevice.h
//Created 20/04/17
//Created By Daniel Bowler
//
//Common interface that each API specific device will inherit from

#pragma once

//Needs to know about the OS window
#include "../../../OSWindow/OSWindow.h"

//May need the rendering instance to create the device
#include "../../Instance/RenderInstance.h"

namespace EngineAPI
{
	namespace Graphics
	{
		class CommandBufferPool;
		class CommandQueueFamily;
		class DeviceMemoryAllocator;
	};
};

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Interface
		{
			class CommonRenderDevice
			{
			public:
				CommonRenderDevice() {};
				virtual ~CommonRenderDevice() = 0 {};

				//Override this shutdown function for each API implementation
				virtual void Shutdown() = 0 {};

			public:
				//Returns the managers
				EngineAPI::Graphics::DeviceMemoryAllocator* GetDeviceMemoryAllocator() { return deviceMemoryAllocator; };

			protected:
				//Device memory manager / allocator
				EngineAPI::Graphics::DeviceMemoryAllocator* deviceMemoryAllocator = nullptr;
			};
		};
	};
};
