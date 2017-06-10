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

	//Invalidate the range of mapped buffer in order to make it visible to the host.
	//if the memory property is set with VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	//then the driver may take care of this, otherwise for non-coherent 
	//mapped memory vkInvalidateMappedMemoryRanges() needs to be called explicitly.
	VkMappedMemoryRange mappedRange = {};
	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.memory = memory;
	mappedRange.offset = 0;
	mappedRange.size = resourceMemoryBlock->GetResourceSize(); //TODO: Actual resource size, not VK assigned size for the buffer (which could be larger than the actual resouce data)

	if (memoryPropsFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT == 0)
	{
		VkResult result = vkInvalidateMappedMemoryRanges(device, 1, &mappedRange);
		assert(result == VK_SUCCESS);
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

	// Flush the range of mapped buffer in order to make it visible to the device
	// If the memory is coherent (memory property must be beVK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	// then the driver may take care of this, otherwise for non-coherent 
	// mapped memory vkFlushMappedMemoryRanges() needs to be called explicitly to flush out 
	// the pending writes on the host side.
	VkMappedMemoryRange mappedRange = {};
	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.memory = memory;
	mappedRange.offset = 0;
	mappedRange.size = resourceMemoryBlock->GetResourceSize(); //TODO: Actual resource size, not VK assigned size for the buffer (which could be larger than the actual resouce data)

	if (memoryPropsFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT == 0)
	{
		VkResult result = vkFlushMappedMemoryRanges(device, 1, &mappedRange);
		assert(result == VK_SUCCESS);
	}

	vkUnmapMemory(device, memory);

	//Block is now unmapped
	resourceMemoryBlock->isMapped = false;
}