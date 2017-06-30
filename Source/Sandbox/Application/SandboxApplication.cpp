#include "SandboxApplication.h"

#include "../App Config/AppConfig.h"

//Scenes
#include "../TestScene/TestScene.h"

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
	//Debug
	EngineAPI::Debug::DebugLog::PrintInfoMessage("SandboxApplication::InitApplication()\n");

	//Inform scene manager of scenes && set first scene
	EngineAPI::Gameplay::SceneManager::GetInstance()->AddNewSceneToManager<TestScene>("Test Scene");
	assert(EngineAPI::Gameplay::SceneManager::GetInstance()->SetCurrentActiveSceneByTitle("Test Scene"));

	//Set clear info
	graphicsSubsystem->SetSwapchainClearValues(UNorm32Colour(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);

	return true;
}

bool SandboxApplication::ShutdownApplication()
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("SandboxApplication::ShutdownApplication()\n");
	return true;
}
