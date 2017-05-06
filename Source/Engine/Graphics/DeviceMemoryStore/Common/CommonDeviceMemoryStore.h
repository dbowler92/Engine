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

//Uses std::vector
#include <vector>

//Manages a set of memory blocks within the store
#include "../../DeviceMemoryBlock/DeviceMemoryBlock.h"

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

				//Override functions to interact with the store
				virtual bool SubAllocMemoryBlock(EUINT_64 blockSize, EngineAPI::Graphics::DeviceMemoryBlock& allocatedBlockOut) = 0;
				virtual void DeallocBlock(const EngineAPI::Graphics::DeviceMemoryBlock* block) = 0;

			public:
				//Getters
				EUINT_64 GetMemoryStoreSizeBytes() { return memoryStoreSizeBytes; };

				//Returns a pointer to the array/vector of blocks
				std::vector<EngineAPI::Graphics::DeviceMemoryBlock>* GetMemoryBlocksArray() { return &memoryBlocksArray; };

			protected:
				//Size of the store in bytes
				EUINT_64 memoryStoreSizeBytes = 0;

				//Pointer to the begining of the store if host visible memory
				void* hostStorePtr = nullptr;

				//Array of blocks - sub allocations from within this block. TODO: Custom 
				//resizing array. 
				std::vector<EngineAPI::Graphics::DeviceMemoryBlock> memoryBlocksArray;
			};
		};
	};
};