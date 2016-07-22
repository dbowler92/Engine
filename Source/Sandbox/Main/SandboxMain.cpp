//SandboxMain.cpp
//Created 22/07/16
//Created By Daniel Bowler
//
//Entry point for the sandbox project

#pragma once

#include <iostream>
#include "../../Engine/Main/EngineStd.h"

int APIENTRY WinMain(HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	//Enter in to engine main
	return EngineMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}