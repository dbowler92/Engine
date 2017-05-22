#include "VulkanResource.h"

//Memory block & store
#include "../../../Graphics/DeviceMemoryStore/DeviceMemoryStore.h"

using namespace EngineAPI::Rendering::Platform;

void* VulkanResource::MapResource()
{
	//Ensure we have a valid memory block before mapping
	assert(resourceMemoryBlock != nullptr);

	//Get data for mapping
	VkDevice device = resourceMemoryBlock->GetParentStore()->GetOwningVKLogicalDevice();
	VkDeviceMemory memory = resourceMemoryBlock->GetParentStore()->GetVKDeviceMemoryHandle();
	VkDeviceSize offset = resourceMemoryBlock->GetBlockAlignedOffsetInStoreBytes();
	VkDeviceSize size = resourceMemoryBlock->GetBlockSizeBytes();
	VkMemoryMapFlags flags = 0;
	void* mappedData = nullptr;
	
	//Map and validate
	VkResult result = vkMapMemory(device, memory, offset, size, flags, &mappedData);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanResource::MapResource() Error: Could not map resource\n");
		return nullptr;
	}

	//Done
	return mappedData;
}

void VulkanResource::UnmapResource()
{
	//Ensure we have a valid memory block before unmapping
	assert(resourceMemoryBlock != nullptr);

	VkDevice device = resourceMemoryBlock->GetParentStore()->GetOwningVKLogicalDevice();
	VkDeviceMemory memory = resourceMemoryBlock->GetParentStore()->GetVKDeviceMemoryHandle();
	vkUnmapMemory(device, memory);
}