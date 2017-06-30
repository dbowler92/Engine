//SceneManager.h
//Created 30/06/17
//Created By Daniel Bowler
//
//Subsystem responsible for scene management. 

#pragma once

#include <string>
#include <list>

//Scene
#include "../Scene/Scene.h"

//Debug
#include "../../Debug/Log/DebugLog.h"

namespace EngineAPI
{
	namespace Gameplay
	{
		class SceneManager
		{
		public:
			//Singleton class
			static SceneManager* GetInstance();
			
		public:
			//Init and shutdown
			bool InitSubsystem();
			void ShutdownSubsystem();

			//Update and render loops
			bool OnUpdate(float dt);
			bool OnRender();

			//Adds a new scene to the scene manager
			template<class T>
			void AddNewSceneToManager(std::string sceneTitle)
			{
				T* newScene = new T();
				newScene->SetSceneTitle(sceneTitle);
				scenes.push_back(newScene);
				newScene->OnAddToSceneManagerList();
			}

			//Sets the current active scene + calls the specific load and unload
			//functions
			bool SetCurrentActiveScene(EngineAPI::Gameplay::Scene* scene);
			bool SetCurrentActiveSceneByTitle(std::string title);

		private:
			//Singleton
			SceneManager();
			~SceneManager();
			SceneManager(SceneManager& other);
			SceneManager& operator= (SceneManager& other);

		private:
			static SceneManager* instance;

		private:
			//List of scenes
			std::list<Scene*> scenes;

			//Current scene
			Scene* currentScene = nullptr;
		};
	}
};