#include "OrbisApplication.h"

#include <iostream> //printf()

//Init global reference to the app. 
EngineAPI::Core::Platform::OrbisApplication *g_App = NULL;

using namespace EngineAPI::Core::Platform;

OrbisApplication::OrbisApplication()
	: appPaused(false), minimized(false), maximized(false), resizing(false)
{}

bool OrbisApplication::InitEngine(int argc, char* argv[],
	int appVersionMajor, int appVersionMinor, int appVersionPatch,
	int screenWidth, int screenHeight)
{
	//   
	//TODO: Plenty more init work that needs to be completed once I get to the relivent
	//chapters in the book. Eg: Registering all engine events
	// 
	EngineAPI::Debug::DebugLog::PrintInfoMessage("OrbisApplication::InitEngine()\n");

	//Store data             
	osWindow.UpdateWindowWidth(screenWidth);
	osWindow.UpdateWindowHeight(screenHeight);
	this->appVersionMajor = appVersionMajor;
	this->appVersionMinor = appVersionMinor;
	this->appVersionPatch = appVersionPatch;

	//Setup window
	bool success = InitOrbisApp();

	//Init engine subsystems - eg: Graphics
	if (success)
		return InitEngineSubsystems();
	else
		return success; //False
}

bool OrbisApplication::ShutdownEngine()
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("OrbisApplication::ShutdownEngine()\n");

	//Shutdown subsystems - reverse order to creation
	graphicsSubsystem->ShutdownSubsystem();

	return true;
}

bool OrbisApplication::InitOrbisApp()
{
	//Done
	return true;
}

bool OrbisApplication::InitEngineSubsystems()
{
	//Init graphics
	graphicsSubsystem = GE_NEW EngineAPI::Graphics::GraphicsManager();
	if (!graphicsSubsystem->InitSubsystem(&osWindow, GetGameTitle(),
		appVersionMajor, appVersionMinor, appVersionPatch,
		osWindow.GetWindowWidth(), osWindow.GetWindowHeight()))
		return false;

	//Done
	return true;
}

void OrbisApplication::OnResize()
{
	//TODO: Resize render targets, etc
}

void OrbisApplication::EnterGameLoop()
{
	mainGameLoopTimer.Reset();
	while (true)
	{
		mainGameLoopTimer.Tick();
		if (!appPaused)
		{
			//Render && update
		}
		else
		{
			//Pause
		}
	}
}