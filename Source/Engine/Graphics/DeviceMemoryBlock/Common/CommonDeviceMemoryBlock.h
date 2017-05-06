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

//Forward declarations
namespace EngineAPI
{
	namespace Graphics
	{
		class DeviceMemoryStore;
	};
};

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

				//Override the init function for the block
				virtual bool Init(EngineAPI::Graphics::DeviceMemoryStore* parentStore, EUINT_64 memoryBlockSize,
					EUINT_64 memoryBlockOffset, bool isBlockMappable) = 0 {};

			public:
				//Getters
				EUINT_64 GetBlockSizeBytes() { return blockSizeBytes; };
				EUINT_64 GetBlockOffsetInStoreBytes() { return blockOffsetInStoreBytes; };

				bool IsBlockFree() { return isFree; };
				bool IsBlockMappable() { return isMappable; };
				bool IsBlockCurrentlyMapped() { return isMapped; };

				void* GetBlockHostMemoryPointer() { return hostBlockPtr; };

			protected:
				EngineAPI::Graphics::DeviceMemoryStore* parentStore = nullptr;

				//Block size in bytes
				EUINT_64 blockSizeBytes = 0;

				//Offset in to the memory store to the start of this block (bytes)
				EUINT_64 blockOffsetInStoreBytes = 0;

				//TODO: Memory block state flags
				bool isFree = true;               //Available to be used to store data (eg: Buffer data, texture data etc)
				bool isMappable = false;		  //Can we map the memory and write/read from it? Depends on the parent store or flags set when allocated in D3D11
				bool isMapped = false;			  //True when we are writing to this block from the CPU

				//If the block is mappable, this pointer can be used
				//to update the memory block
				void* hostBlockPtr = nullptr;
			};
		};
	};
};