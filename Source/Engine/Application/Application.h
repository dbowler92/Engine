//Application.h
//Created 23/07/16
//Created by Daniel Bowler
//
//Application layer of the engine. Each game that uses this engine
//should define a game specific class that inherits from this
//base application class & implement the virtual functions.

#pragma once

#include <Windows.h>
#include "../Main/EngineStd.h"

namespace EngineAPI
{
	class Application
	{
		HIDE_COPY_ASSIGNMENT(Application)
	public:
		Application();
		
	public:

	};
};

//Define a global pointer to the singleton app instance. Override Application
//and provide a project specific version
extern EngineAPI::Application* g_App;