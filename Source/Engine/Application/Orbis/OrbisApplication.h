//OrbisApplication.h
//Created 15/14/17
//Created By Daniel Bowler
//
//PS4 implementation of the base application class

#include "../Common/CommonApplication.h"
#include "../../Utils/GameTimer/GameTimer.h"

namespace EngineAPI
{
	namespace Core
	{
		namespace Platform
		{
			class OrbisApplication : public EngineAPI::Core::Interface::CommonApplication
			{
				HIDE_COPY_ASSIGNMENT(OrbisApplication)
			public:
				OrbisApplication();

				//
				//Virtual functions. Game specific instance of this class should implement these
				//
				virtual ECHAR *GetGameTitle() = 0;

				//Inits your game - called after engine and subsystems have been 
				//inited. Here, you would create your game specific subsystems and
				//assets etc. 
				virtual bool InitApplication() = 0;

				//Called when the engine is shutdown before closing the actual engine. 
				virtual bool ShutdownApplication() = 0;

			public:
				//Engine
				//
				//Init engine & engine subsystems. Interacts with PS4 SDK. Called before initing the 
				//game 
				bool InitEngine(int argc, char* argv[], int screenWidth = 960, int screenHeight = 540);

				//Shutsdown the engine - called after shutting down the 
				bool ShutdownEngine();

				//Game loop
				void EnterGameLoop();

			private:
				//Inits Orbis application
				bool InitOrbisApp();

				//Inits the engine subsystems - eg: Graphics, physics, etc
				bool InitEngineSubsystems();

				//Called when the window is resized
				void OnResize();

			private:
				unsigned appWidth, appHeight; //Dimentions of application window				
				bool appPaused, minimized, maximized, resizing; //State of app
				Utils::GameTimer mainGameLoopTimer; //Calculates frame delta
			};
		};
	}; 
};

//Global application pointer. 
extern EngineAPI::Core::Platform::OrbisApplication* g_App;