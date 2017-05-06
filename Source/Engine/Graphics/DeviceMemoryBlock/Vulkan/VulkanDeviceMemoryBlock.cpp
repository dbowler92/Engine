#include "VulkanDeviceMemoryBlock.h"

//Complete forward declarations
#include "../../DeviceMemoryStore/DeviceMemoryStore.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanDeviceMemoryBlock::Shutdown()
{}

bool VulkanDeviceMemoryBlock::Init(EngineAPI::Graphics::DeviceMemoryStore* parentStore, EUINT_64 memoryBlockSize,
	EUINT_64 memoryBlockOffset, bool isBlockMappable)
{
	//Cache data for this block
	this->parentStore = parentStore;
	this->parentVKDeviceMemory = parentStore->GetVKDeviceMemoryHandle();
	this->blockSizeBytes = memoryBlockSize;
	this->blockOffsetInStoreBytes = memoryBlockOffset;
	this->isMappable = isBlockMappable;

	//Done
	return true;
}