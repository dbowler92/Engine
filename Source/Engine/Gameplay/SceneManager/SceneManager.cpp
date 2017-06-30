#include "SceneManager.h"

//Assert 
#include <assert.h>

using namespace EngineAPI::Gameplay;

SceneManager* SceneManager::instance = nullptr;

SceneManager::SceneManager()
{}

SceneManager::~SceneManager()
{}

SceneManager* SceneManager::GetInstance()
{
	if (!instance)
		instance = new SceneManager();
	return instance;
}

void SceneManager::ShutdownSubsystem()
{
	//Deactivate current scene
	if (currentScene)
		assert(currentScene->OnSceneBecomeDeactive());

	//Cleanup scenes
	std::list<Scene*>::iterator it = scenes.begin();
	while (it != scenes.end())
	{
		assert((*it)->OnEngineShutdown());
		delete *it;
		it++;
	}
	scenes.clear();

	if (SceneManager::instance)
	{
		delete instance;
		SceneManager::instance = nullptr;
	}
}

bool SceneManager::InitSubsystem()
{
	//Done
	return true;
}

bool SceneManager::OnUpdate(float dt)
{
	//Update current scene
	if (currentScene)
		return currentScene->OnSceneUpdate(dt);

	//Done
	return true;
}

bool SceneManager::OnRender()
{
	//Render current scene
	if (currentScene)
		return currentScene->OnSceneRender();

	//Done
	return true;
}

bool SceneManager::SetCurrentActiveScene(EngineAPI::Gameplay::Scene* scene)
{
	if (scene == nullptr)
		return false;

	if (currentScene)
		assert(currentScene->OnSceneBecomeDeactive());

	currentScene = scene;
	assert(currentScene->OnSceneBecomeActive());

	//Done
	return true;
}

bool SceneManager::SetCurrentActiveSceneByTitle(std::string title)
{
	//Find scene in list
	std::list<Scene*>::iterator it = scenes.begin();
	while (it != scenes.end())
	{
		std::string sceneTitle = (*it)->GetSceneTitle();
		if (sceneTitle == title)
		{
			if (currentScene)
				assert(currentScene->OnSceneBecomeDeactive());

			currentScene = *it;
			assert(currentScene->OnSceneBecomeActive());

			//Done
			return true;
		}

		//next scene in list
		++it;
	}

	//Failed
	EngineAPI::Debug::DebugLog::PrintErrorMessage("SceneManager::SetCurrentActiveSceneByTitle() Error - Could not find scene");
	return false;
}