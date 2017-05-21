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
	streams[0].Binding = 0;
	streams[0].Format = VK_FORMAT_R32G32B32A32_SFLOAT;
	streams[0].ShaderBindingLocation = 0;
	streams[0].Offset = 0;

	streams[1].Binding = 0;
	streams[1].Format = VK_FORMAT_R32G32B32A32_SFLOAT;
	streams[1].ShaderBindingLocation = 1;
	streams[1].Offset = sizeof(float) * 4;

	//VB Description
	VertexBufferLayout vbLayout = {};
	vbLayout.Usage = VERTEX_BUFFER_USAGE_PER_VERTEX_DATA;
	vbLayout.VertexStreamsCount = 2;
	vbLayout.VertexStreams = streams;

	vb.SetResourceDebugName("Test Vertex Buffer");
	assert(vb.InitVKVertexBuffer(device, sizeof(triangleData), sizeof(VertexWithColour), false));
	assert(vb.AllocAndBindVKVertexBuffer(device, &vbLayout, &triangleData[0], nullptr));

	//TEMP
	device->GetDeviceMemoryAllocator()->Debug_LongDump("Dumps/PostVB");

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