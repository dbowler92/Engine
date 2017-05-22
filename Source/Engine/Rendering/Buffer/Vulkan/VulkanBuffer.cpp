#include "VulkanBuffer.h"

//Memory allocator
#include "../../../Graphics/DeviceMemoryAllocator/DeviceMemoryAllocator.h"

using namespace EngineAPI::Rendering::Platform;

void VulkanBuffer::Shutdown()
{
	vkDestroyBuffer(cachedVKLogicalDevice, vkBufferHandle, nullptr);
	vkBufferHandle = VK_NULL_HANDLE;
}

bool VulkanBuffer::InitVKBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
	VkBufferCreateInfo* bufferCreateInfo, bool isDynamicBuffer)
{
	//Cache info
	this->cachedVKLogicalDevice = renderingDevice->GetVKLogicalDevice();
	this->isDynamicResourceFlag = isDynamicBuffer;
	this->bufferContentsSizeBytes = bufferCreateInfo->size; //Contents size, not the actual size of the buffer (which maybe larger for alignment purposes)! 

	//Create the buffer object
	VkResult result = vkCreateBuffer(cachedVKLogicalDevice, bufferCreateInfo, nullptr, &vkBufferHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanBuffer::InitVKBuffer() Error - Could not create VkBuffer object\n");
		return false;
	}

	//Get memory requirements for the buffer once available.
	vkGetBufferMemoryRequirements(cachedVKLogicalDevice, vkBufferHandle, &vkResourceMemoryRequirments);

	//Fill VK buffer description
	vkBufferInfo.buffer = vkBufferHandle;
	vkBufferInfo.range = vkResourceMemoryRequirments.size;
	vkBufferInfo.offset = 0; //VERIFY

	//Done
	return true;
}

bool VulkanBuffer::AllocVKBufferMemoryBlock(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Graphics::DeviceMemoryStore* optionalMemoryStore)
{
	//Get allocator
	EngineAPI::Graphics::DeviceMemoryAllocator* memoryAllocator = renderingDevice->GetDeviceMemoryAllocator();
	
	//Alloc
	if (optionalMemoryStore != nullptr)
	{
		//Use existing store if provided
		SuballocationResult result = memoryAllocator->AllocateResourceToStore(renderingDevice, optionalMemoryStore, vkResourceMemoryRequirments, this);
		if (result != ALLOCATION_RESULT_SUCCESS)
		{
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanBuffer::AllocAndBindVKBufferMemory(): Error allocating memory for this texture (Custom defined store)\n");
			return false; 
		}
	}
	else
	{
		//Else, let the engine decide how to allocate
		SuballocationResult result = memoryAllocator->AllocateResourceAuto(renderingDevice, this);
		if (result != ALLOCATION_RESULT_SUCCESS)
		{
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanBuffer::AllocAndBindVKTextureMemory(): Error allocating memory for this buffer\n");
			return false;
		}
	}

	//Done
	return true;
}

bool VulkanBuffer::WriteDataToVKBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice, void* data)
{
	//
	//TODO: Staging buffers for non directly mappable resources (eg: GPU only buffers)
	//

	//Map the resource
	void* mappedResource = MapResource();
	if (mappedResource)
	{
		//Success! Write data
		memcpy(mappedResource, data, bufferContentsSizeBytes);

		//Unmap
		UnmapResource();
		return true;
	}
	else
	{
		//Failed for some reason. Unmap resource just incase and return
		UnmapResource();
		return false;
	}
}

bool VulkanBuffer::BindVKBufferMemory(EngineAPI::Graphics::RenderDevice* renderingDevice)
{
	//Bind allocated buffer resource to device memory
	VkDevice device = renderingDevice->GetVKLogicalDevice();
	VkDeviceMemory memory = resourceMemoryBlock->GetParentStore()->GetVKDeviceMemoryHandle();
	VkDeviceSize offset = resourceMemoryBlock->GetBlockAlignedOffsetInStoreBytes();
	VkResult result = vkBindBufferMemory(device, vkBufferHandle, memory, offset);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanBuffer::BindVKBufferMemory() Error: Could not bind memory to device\n");
		return false;
	}

	//Done
	return true;
}