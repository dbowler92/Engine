#include "VulkanIndexBuffer.h"

using namespace EngineAPI::Rendering::Platform;

void VulkanIndexBuffer::Shutdown()
{
	//Shutdown super class - the buffer object
	__super::Shutdown();
}

VkIndexType VulkanIndexBuffer::GetVKIndexType()
{
	VkIndexType type;
	(indexBufferStride == INDEX_ELEMENT_DATA_SIZE_UINT_16) ? type = VK_INDEX_TYPE_UINT16 : type = VK_INDEX_TYPE_UINT32;
	return type;
}

bool VulkanIndexBuffer::InitVKIndexBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
	VkDeviceSize indexBufferSizeBytes, bool isDynamicIndexBuffer, RenderingResourceUsage resourceUsage)
{
	//Creation info
	VkBufferCreateInfo ibCreateInfo = {};
	ibCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	ibCreateInfo.pNext = nullptr;
	ibCreateInfo.flags = 0; //TODO
	ibCreateInfo.size = indexBufferSizeBytes;
	ibCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT; //VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
	ibCreateInfo.pQueueFamilyIndices = nullptr;
	ibCreateInfo.queueFamilyIndexCount = 0;
	ibCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	//Init buffer
	if (!InitVKBuffer(renderingDevice, &ibCreateInfo, resourceUsage))
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanIndexBuffer::InitVKIndexBuffer() Error - Could not init VkBuffer\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanIndexBuffer::AllocAndBindVKIndexBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
	IndexBufferLayout* indexBufferLayout, void* indexBufferData,
	EngineAPI::Graphics::DeviceMemoryStore* optionalDeviceStore)
{
	assert(vkBufferHandle != VK_NULL_HANDLE);

	//Cache info
	this->indexBufferStride = indexBufferLayout->IndexStride;

	//Calculate number of elements in the IB
	uint32_t indexBufferStrideBytes = 0;
	(indexBufferStride == INDEX_ELEMENT_DATA_SIZE_UINT_16) ? indexBufferStrideBytes = 2 : indexBufferStrideBytes = 4;
	indexBufferElementsCount = bufferContentsSizeBytes / indexBufferStrideBytes;

	//Alloc resource block through the DeviceMemoryAllocator
	if (!AllocVKBufferMemoryBlock(renderingDevice, optionalDeviceStore))
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanIndexBuffer::AllocAndBindVKIndexBuffer() Error - Could not alloc memory block\n");
		return false;
	}

	//Staging -> Write data to the buffer
	if (indexBufferData)
	{
		if (!WriteDataToVKBuffer(renderingDevice, indexBufferData))
		{
			//Error
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanIndexBuffer::AllocAndBindVKIndexBuffer() Error - Could not write data to memory(Staging)\n");
			return false;
		}
	}

	//Bind device memory
	if (!BindVKBufferMemory(renderingDevice))
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanIndexBuffer::AllocAndBindVKIndexBuffer() Error - Could not bind memory on device\n");
		return false;
	}

	//Done
	return true;
}


bool VulkanIndexBuffer::InitVKBufferViews(EngineAPI::Graphics::RenderDevice* renderingDevice)
{
	//TODO

	//Done
	return true;
}