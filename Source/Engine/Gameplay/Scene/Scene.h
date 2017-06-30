//Scene.h
//Created 30/06/17
//Created By Daniel Bowler
//
//Represents a single scene within the game

#pragma once

#include <string>	

//Debug log
#include "../../Debug/Log/DebugLog.h"

//Includes
#include "../../Includes/RenderingIncludes.h"

//Managers
#include "../../Graphics/Manager/GraphicsManager.h"

namespace EngineAPI
{
	namespace Gameplay
	{
		class Scene
		{
			friend class SceneManager;

		public:
			//Gets the scene title.
			std::string GetSceneTitle() { return sceneTitle; };

		protected:
			Scene();
			~Scene();

			//Load/Unload events. 
			virtual bool OnAddToSceneManagerList() = 0;
			virtual bool OnSceneBecomeActive() = 0;
			virtual bool OnSceneBecomeDeactive() = 0;
			virtual bool OnRemoveFromSceneManagerList() = 0;
			virtual bool OnEngineShutdown() = 0;

			//Update and rendering events that we override
			virtual bool OnSceneUpdate(float dt) = 0;
			virtual bool OnSceneRender() = 0;

		protected:
			//Scene title - called by scene manager when added to the managers
			//list
			void SetSceneTitle(std::string title) { sceneTitle = title; };
			std::string sceneTitle;
		};
	};
};
