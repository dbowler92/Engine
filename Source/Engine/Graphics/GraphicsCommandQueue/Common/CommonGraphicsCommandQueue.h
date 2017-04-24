//CommonGraphicsQueue.h
//Created 24/04/17
//Created By Daniel Bowler
//
//Base class that platform specifc abstractions will inherit from

#pragma once

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Interface
		{
			class CommonGraphicsCommandQueue
			{
			public:
				CommonGraphicsCommandQueue() {};
				virtual ~CommonGraphicsCommandQueue() = 0 {};

				//Override shutdown function for cleanup
				virtual void Shutdown() = 0 {};
			};
		};
	};
};
