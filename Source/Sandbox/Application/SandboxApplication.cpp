#include "SandboxApplication.h"

#include <iostream> //printf()
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
	
	assert(vb.InitVKVertexBuffer(device, sizeof(triangleData), sizeof(VertexWithColour), false));
	assert(vb.AllocAndBindVKVertexBuffer(device, &triangleData[0]));
	assert(vb.InitVKVertexBufferViews(device));

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