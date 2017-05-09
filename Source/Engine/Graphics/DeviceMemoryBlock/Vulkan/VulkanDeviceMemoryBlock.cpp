#include "VulkanDeviceMemoryBlock.h"

//Complete forward declarations
#include "../../DeviceMemoryStore/DeviceMemoryStore.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanDeviceMemoryBlock::Shutdown()
{
	//Block is free!
	this->isFree = true;

	//Free resource?
	//
	//

	//Not pointing to a resource anymore
	this->resourcePtr = nullptr;
}

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

	//Block is being used
	this->isFree = false;

	//Tell the resource that it now has a memory block
	this->resourcePtr->Private_SetDeviceMemoryBlockPointer((DeviceMemoryBlock*)this);

	//Done
	return true;
}

void VulkanDeviceMemoryBlock::FreeMemoryBlock()
{
	//Block is free!
	this->isFree = true;

	//Free resource?
	//
	//

	//Not pointing to a resource anymore
	this->resourcePtr->Private_SetDeviceMemoryBlockPointer(nullptr);
	this->resourcePtr = nullptr;
}