#include "SandboxApplication.h"

//Memory allocator
#include "../../Engine/Graphics/DeviceMemoryAllocator/DeviceMemoryAllocator.h"

#include "../App Config/AppConfig.h"

SandboxApplication::SandboxApplication()
{}

ECHAR* SandboxApplication::GetGameTitle()
{
	return "Sandbox";
/*
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
	return "Sandbox (Vulkan)"; 
#endif
*/
};

bool SandboxApplication::InitApplication()
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("SandboxApplication::InitApplication()\n");

	//Device -> Used when creating rendering resources
	EngineAPI::Graphics::RenderDevice* device = EngineAPI::Graphics::GraphicsManager::GetInstance()->GetRenderingDevice();

	//Test Vertex buffer object:
	struct VertexWithColour
	{
		float x, y, z, w; // Vertex Position
		float r, g, b, a; // Colour format Red, Green, Blue, Alpha
	};

	VertexWithColour triangleData[] =
	{
		{ 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
		{ 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
		{ -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
	};

	//Streams
	VertexStreamDescription streams[2];
	streams[0].BufferBinding = 0;
	streams[0].Format = VK_FORMAT_R32G32B32A32_SFLOAT;
	streams[0].ShaderBinding = 0;
	streams[0].Offset = 0;

	streams[1].BufferBinding = 0;
	streams[1].Format = VK_FORMAT_R32G32B32A32_SFLOAT;
	streams[1].ShaderBinding = 1;
	streams[1].Offset = sizeof(float) * 4;

	//VB Description
	VertexBufferLayout vbLayout = {};
	vbLayout.Usage = VERTEX_BUFFER_USAGE_PER_VERTEX_DATA;
	vbLayout.BufferBinding = 0;
	vbLayout.VertexStride = sizeof(VertexWithColour);
	vbLayout.VertexStreamsCount = 2;
	vbLayout.VertexStreams = streams;

	bool isDynamicVB = true; //TEMP: For VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT memory rather than GPU only. 
	EngineAPI::Graphics::DeviceMemoryStore* customStoreForVB = nullptr;

	vb.SetResourceDebugName("Test Vertex Buffer");
	assert(vb.InitVKVertexBuffer(device, sizeof(triangleData), isDynamicVB));
	assert(vb.AllocAndBindVKVertexBuffer(device, &vbLayout, &triangleData[0], customStoreForVB));

	//TEMP
	device->GetDeviceMemoryAllocator()->Debug_LongDump("Dumps/PostVB_LabPC_DynamicVB");

	return true;
}

bool SandboxApplication::ShutdownApplication()
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("SandboxApplication::ShutdownApplication()\n");
	
	//Shutdown rendering data
	vb.Shutdown();

	return true;
}

void SandboxApplication::UpdateScene(float dt)
{

}

void SandboxApplication::RenderScene()
{
	
}