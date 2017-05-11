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
	VkDeviceSize memoryAlignmentRequirments, VkDeviceSize memoryBlockSize, VkDeviceSize memoryBlockOffset,
	VkDeviceSize resourceSize,
	bool isBlockMappable)
{
	//Cache data for this block
	this->parentStore = parentStore;
	this->resourcePtr = resource;
	this->resourceSizeBytes = resourceSize;
	this->parentVKDeviceMemory = parentStore->GetVKDeviceMemoryHandle();
	this->resourceAlignment = memoryAlignmentRequirments;
	this->blockSizeBytes = memoryBlockSize;
	this->blockOffsetInStoreBytes = memoryBlockOffset;
	this->isMappable = isBlockMappable;

	//Block is being used
	this->isFree = false;

	//Calculate aligned memory offset for this suballocation - shift to the right
	VkDeviceSize missAlignment = (memoryBlockOffset % memoryAlignmentRequirments);
	VkDeviceSize bytesToShiftRightToMakeAligned = 0; //Could already be aligned
	if (missAlignment != 0)
		bytesToShiftRightToMakeAligned = memoryAlignmentRequirments - missAlignment;
	this->blockAlignedOffsetInStoreBytes = memoryBlockOffset + bytesToShiftRightToMakeAligned;

	//Tell the resource that it now has a memory block
	if (this->resourcePtr)
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
	if (this->resourcePtr)
		this->resourcePtr->Private_SetDeviceMemoryBlockPointer(nullptr);
	this->resourcePtr = nullptr;
}