#include "VulkanVertexBuffer.h"

using namespace EngineAPI::Rendering::Platform;

void VulkanVertexBuffer::Shutdown()
{
	//Shutdown vertex buffer data - eg: views

	//Shutdown super class - the buffer object
	__super::Shutdown();
}

bool VulkanVertexBuffer::InitVKVertexBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
	VkDeviceSize vertexBufferSizeBytes, void* vertexBufferData,
	bool isDynamicVertexBuffer)
{
	//Creation info
	VkBufferCreateInfo vbCreateInfo = {};
	vbCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vbCreateInfo.pNext = nullptr;
	vbCreateInfo.flags = 0; //TODO
	vbCreateInfo.size = vertexBufferSizeBytes;
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