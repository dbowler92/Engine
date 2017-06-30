#include "TestScene.h"

bool TestScene::OnAddToSceneManagerList()
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("TestScene::OnAddToSceneManagerList()\n");

	//Done
	return true;
}

bool TestScene::OnSceneBecomeActive()
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("TestScene::OnSceneBecomeActive()\n");

	EngineAPI::Graphics::GraphicsManager* graphicsSubsystem = EngineAPI::Graphics::GraphicsManager::GetInstance();
	
	//tri.Init(graphicsSubsystem);
	//quad.Init(graphicsSubsystem);
	tCube.Init(graphicsSubsystem);
	//pConstCube.Init(graphicsSubsystem);

#if TEXTURED_CUBE_DO_USE_OPTIMAL_TILING_FOR_SAMPLER
	graphicsSubsystem->GetRenderingDevice()->GetDeviceMemoryAllocator()->Debug_LongDump(DEBUG_DUMPS_FOLDER"TexturedCubeOptimal");
#else
	graphicsSubsystem->GetRenderingDevice()->GetDeviceMemoryAllocator()->Debug_LongDump(DEBUG_DUMPS_FOLDER"TexturedCubeLinear");
#endif

	//Done
	return true;
}

bool TestScene::OnSceneBecomeDeactive()
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("TestScene::OnSceneBecomeDeactive()\n");

	//tri.Shutdown();
	//quad.Shutdown();
	tCube.Shutdown();
	//pConstCube.Shutdown();

	//Done
	return true;
}

bool TestScene::OnRemoveFromSceneManagerList()
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("TestScene::OnRemoveFromSceneManagerList()\n");

	//Done
	return true;
}

bool TestScene::OnEngineShutdown()
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("TestScene::OnFinalShutdown()\n");

	//Done
	return true;
}

bool TestScene::OnSceneUpdate(float dt)
{
	tCube.Update(dt);
	//pConstCube.Update(dt);

	//Done
	return true;
}

bool TestScene::OnSceneRender()
{
	EngineAPI::Graphics::GraphicsManager* graphicsSubsystem = EngineAPI::Graphics::GraphicsManager::GetInstance();

	//Render something...
	//tri.GenerateRenderingCommands(graphicsSubsystem);
	//quad.GenerateRenderingCommands(graphicsSubsystem);
	tCube.GenerateRenderingCommands(graphicsSubsystem);
	//pConstCube.GenerateRenderingCommands(graphicsSubsystem);
	
	//Done
	return true;
}
