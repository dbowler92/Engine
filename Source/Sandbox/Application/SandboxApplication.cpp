#include "SandboxApplication.h"

#include "../App Config/AppConfig.h"

//Memory allocator
#include "../../Engine/Graphics/DeviceMemoryAllocator/DeviceMemoryAllocator.h"

//Statics 
#include "../../Engine/Statics/Vulkan/VulkanStatics.h"

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

	//Set clear info
	graphicsSubsystem->SetSwapchainClearValues(UNorm32Colour(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);

	//Device -> Used when creating rendering resources
	EngineAPI::Graphics::RenderDevice* device = EngineAPI::Graphics::GraphicsManager::GetInstance()->GetRenderingDevice();

	//Init triangle
	//tri.Init(graphicsSubsystem);

	//Init quad
	//quad.Init(graphicsSubsystem);

	//Init textured quad
	tCube.Init(graphicsSubsystem);
	
	//graphicsSubsystem->GetRenderingDevice()->GetDeviceMemoryAllocator()->Debug_LongDump(DEBUG_DUMPS_FOLDER"UniformBuffer");

	return true;
}

bool SandboxApplication::ShutdownApplication()
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("SandboxApplication::ShutdownApplication()\n");
	
	//
	//Triangle
	//
	//tri.Shutdown();

	//
	//Quad
	//
	//quad.Shutdown();

	//
	//Textured Quad
	//
	tCube.Shutdown();

	return true;
}

void SandboxApplication::UpdateScene(float dt)
{
	static float timer = 0.0f;
	timer += dt;
	
	if (timer >= 1.0f)
	{
		//Switch rendering
		doRenderTriangle = !doRenderTriangle;
		timer -= 1.0f;

		//TODO: Mark rendering as dirty -> Render pass instance
		//command buffers need to be reset and reinited! 
	}

	tCube.Update(dt);
}

void SandboxApplication::RenderScene()
{
	//Render something...
	//tri.GenerateRenderingCommands(graphicsSubsystem);
	//quad.GenerateRenderingCommands(graphicsSubsystem);
	tCube.GenerateRenderingCommands(graphicsSubsystem);
}