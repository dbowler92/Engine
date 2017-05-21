#include "VulkanVertexBuffer.h"

using namespace EngineAPI::Rendering::Platform;

void VulkanVertexBuffer::Shutdown()
{
	//Shutdown vertex buffer data - eg: views
	inputAttributes.clear();

	//Shutdown super class - the buffer object
	__super::Shutdown();
}

bool VulkanVertexBuffer::InitVKVertexBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
	VkDeviceSize vertexBufferSizeBytes, bool isDynamicVertexBuffer)
{
	//Creation info
	VkBufferCreateInfo vbCreateInfo = {};
	vbCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vbCreateInfo.pNext = nullptr;
	vbCreateInfo.flags = 0; //TODO
	vbCreateInfo.size  = vertexBufferSizeBytes;
	vbCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; //VERIFY
	vbCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	//Init buffer
	if (!InitVKBuffer(renderingDevice, &vbCreateInfo, isDynamicVertexBuffer))
	{
		//Erorr
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanVertexBuffer::InitVKVertexBuffer() Error - Could not init VkBuffer\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanVertexBuffer::AllocAndBindVKVertexBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice, 
	VertexBufferLayout* vertexBufferLayout, void* vertexBufferData, 
	EngineAPI::Graphics::DeviceMemoryStore* optionalDeviceStore)
{
	assert(vkBufferHandle != VK_NULL_HANDLE);

	//Cache some data
	this->vertexBufferStride = vertexBufferLayout->VertexStride;

	//Calculate number of elements in the VB
	vertexBufferElementsCount = bufferSizeBytes / vertexBufferStride;

	//Fill VK structs
	//
	//VkVertexInputBindingDescription
	bufferBinding.binding = vertexBufferLayout->BufferBinding;
	bufferBinding.stride = vertexBufferStride;
	if (vertexBufferLayout->Usage == VERTEX_BUFFER_USAGE_PER_VERTEX_DATA)
		bufferBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	else
		bufferBinding.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

	//VkVertexInputAttributeDescription[]
	inputAttributes.resize(vertexBufferLayout->VertexStreamsCount);
	for (int i = 0; i < vertexBufferLayout->VertexStreamsCount; i++)
	{
		inputAttributes[i].binding  = vertexBufferLayout->VertexStreams[i].BufferBinding;
		inputAttributes[i].format   = vertexBufferLayout->VertexStreams[i].Format;
		inputAttributes[i].location = vertexBufferLayout->VertexStreams[i].ShaderBinding;
		inputAttributes[i].offset	= vertexBufferLayout->VertexStreams[i].Offset;
	}

	//Alloc resource block through the DeviceMemoryAllocator
	if (!AllocVKBufferMemoryBlock(renderingDevice, optionalDeviceStore))
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanVertexBuffer::AllocAndBindVKVertexBuffer() Error - Could not alloc memory block\n");
		return false;
	}

	//Staging -> Write data to the buffer
	if (vertexBufferData)
	{
		if (!WriteDataToVKBuffer(renderingDevice, vertexBufferData))
		{
			//Error
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanVertexBuffer::AllocAndBindVKVertexBuffer() Error - Could not write data to memory(Staging)\n");
			return false;
		}
	}

	//Bind device memory
	if (!BindVKBufferMemory(renderingDevice))
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanVertexBuffer::AllocAndBindVKVertexBuffer() Error - Could not bind memory on device\n");
		return false;
	}

	//Done
	return true;
}