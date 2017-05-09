#include "VulkanDeviceMemoryBlock.h"

//Complete forward declarations
#include "../../DeviceMemoryStore/DeviceMemoryStore.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanDeviceMemoryBlock::Shutdown()
{}

bool VulkanDeviceMemoryBlock::InitVKMemoryBlock(
	EngineAPI::Graphics::DeviceMemoryStore* parentStore, 
	EngineAPI::Rendering::Resource* resource,
	VkDeviceSize memoryBlockSize, VkDeviceSize memoryBlockOffset, bool isBlockMappable)
{
	//Cache data for this block
	this->parentStore = parentStore;
	this->resourcePtr = resource;
	this->parentVKDeviceMemory = parentStore->GetVKDeviceMemoryHandle();
	this->blockSizeBytes = memoryBlockSize;
	this->blockOffsetInStoreBytes = memoryBlockOffset;
	this->isMappable = isBlockMappable;

	//Done
	return true;
}