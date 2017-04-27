//CommonCommandQueueFamily.h
//Created 24/04/17
//Created By Daniel Bowler
//
//Base class that platform specifc abstractions will inherit from

#pragma once

//Manages specific command queues
#include "../../CommandQueue/CommandQueue.h"

//Debug
#include "../../../Debug/Log/DebugLog.h"

//What are the queues job within this family?
enum QueueFamilySupport
{
	QUEUE_FAMILY_SUPPORT_NULL,                //Not inited yet.
	QUEUE_FAMILY_SUPPORT_GRAPHICS,
	QUEUE_FAMILY_SUPPORT_COMPUTE,
	QUEUE_FAMILY_SUPPORT_DMA_TRANSFER,
	QUEUE_FAMILY_SUPPORT_SPARSE,
	QUEUE_FAMILY_SUPPORT_ALL_GRAPHICS_WORK    //D3D11 etc -> Probably just have one queue family with one queue doing everything...
};

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Interface
		{
			class CommonCommandQueueFamily
			{
			public:
				CommonCommandQueueFamily() {};
				virtual ~CommonCommandQueueFamily() = 0 {};

				//Override shutdown function for cleanup
				virtual void Shutdown() = 0 {};

			protected:
				//Will contain and manage a set of command queues
				CommandQueue* commandQueuesArray = nullptr;

				//Info on this queue family
				QueueFamilySupport queueFamilyRole = QUEUE_FAMILY_SUPPORT_NULL;
				unsigned commandQueuesCount = 0;
			};
		};
	};
};
