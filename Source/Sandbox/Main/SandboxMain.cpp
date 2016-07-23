//SandboxMain.cpp
//Created 22/07/16
//Created By Daniel Bowler
//
//Entry point for the sandbox project

#pragma once

#include <iostream>
#include "../../Engine/Main/EngineStd.h" //Allows us to easily call entey function for Engine lib

//Declare application specific instances of engine classes (such as the base Application class) 
//globally (so they are allocated in static memory). These should then be passed to the engine
//libray. 

int APIENTRY WinMain(HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPSTR     lpCmdLine, 
	int       nCmdShow)
{

	//Enter in to engine main
	return EngineMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}