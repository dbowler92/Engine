//SandboxMain.cpp
//Created 22/07/16
//Created By Daniel Bowler
//
//Entry point for the sandbox project

#pragma once

#include "../../Engine/Main/EngineStd.h" //Allows us to easily call entey function for Engine lib
#include "../../Sandbox/Application/SandboxApplication.h"

//Declare application specific instances of engine classes (such as the base Application class) 
//globally (so they are allocated in static memory). These should then be passed to the engine
//libray. 
SandboxApplication g_GameApp; //Implement virtual functions to add game logic layer and view(s)

INT APIENTRY wWinMain(HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPWSTR     lpCmdLine,  
	int       nCmdShow)    
{
	//Enter in to engine main passing the gane specific applcation
	return EngineMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow, 
		&g_GameApp); 
}                