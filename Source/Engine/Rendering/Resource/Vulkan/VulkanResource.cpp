#include "VulkanResource.h"

//Memory block & store
#include "../../../Graphics/DeviceMemoryStore/DeviceMemoryStore.h"

using namespace EngineAPI::Rendering::Platform;

void VulkanResource::Shutdown()
{
	//Free memory block data
	if (resourceMemoryBlock)
		resourceMemoryBlock->FreeMemoryBlock();
	
}

void* VulkanResource::MapResource()
{
	//Ensure we have a valid memory block before mapping
	assert(resourceMemoryBlock != nullptr);

	//Get data for mapping
	VkDevice device = resourceMemoryBlock->GetParentStore()->GetOwningVKLogicalDevice();
	VkDeviceMemory memory = resourceMemoryBlock->GetParentStore()->GetVKDeviceMemoryHandle();
	VkDeviceSize offset = resourceMemoryBlock->GetBlockAlignedOffsetInStoreBytes();
	VkDeviceSize size = resourceMemoryBlock->GetBlockSizeBytes();

	VkMemoryPropertyFlags memoryPropsFlags = resourceMemoryBlock->GetParentStore()->GetStoreMemoryPropertyFlags();

	VkMemoryMapFlags flags = 0;
	void* mappedData = nullptr;
	
	//Map and validate
	VkResult result = vkMapMemory(device, memory, offset, size, flags, &mappedData);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanResource::MapResource() Error: Could not map resource\n");
		return nullptr;
	}

	//Data is mapped
	resourceMemoryBlock->isMapped = true;

	//Done
	return mappedData;
}

void VulkanResource::UnmapResource()
{
	//Ensure we have a valid memory block before unmapping
	assert(resourceMemoryBlock != nullptr);

	VkDevice device = resourceMemoryBlock->GetParentStore()->GetOwningVKLogicalDevice();
	VkDeviceMemory memory = resourceMemoryBlock->GetParentStore()->GetVKDeviceMemoryHandle();

	VkMemoryPropertyFlags memoryPropsFlags = resourceMemoryBlock->GetParentStore()->GetStoreMemoryPropertyFlags();

	vkUnmapMemory(device, memory);

	//Block is now unmapped
	resourceMemoryBlock->isMapped = false;
}