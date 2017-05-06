//SandboxMain.cpp
//Created 22/07/16
//Created By Daniel Bowler
//
//Entry point for the sandbox project

#pragma once

#include "../App Config/AppConfig.h"
#include "../../Engine/Config/EngineConfig.h" //Build settings etc
#include "../../Engine/Main/EngineStd.h" //Allows us to easily call entey function for Engine lib
#include "../../Sandbox/Application/SandboxApplication.h" //Our application
#include <iostream>

//Declare application specific instances of engine classes (such as the base Application class) 
//globally (so they are allocated in static memory). These should then be passed to the engine
//lib 
SandboxApplication g_SandboxApp; //Implement virtual functions to add game logic layer and view(s)

#ifdef ENGINE_CONFIG_PLATFORM_WIN32
INT WINAPI wWinMain(HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPWSTR     lpCmdLine,  
	int       nCmdShow)    
{
	//Enter in to engine main passing the game specific applcation
	return EngineMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow, 
		APPLICATION_VERSION_MAJOR, APPLICATION_VERSION_MINOR, APPLICATION_VERSION_PATCH,
		&g_SandboxApp);
}                
#endif 