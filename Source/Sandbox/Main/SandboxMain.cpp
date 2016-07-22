//SandboxMain.cpp
//Created 22/07/16
//Created By Daniel Bowler
//
//Entry point for the sandbox project

#pragma once

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>

int APIENTRY WinMain(HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	//Enter in to engine
	//EngineMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	printf("WinMain::Main()");

	//Done   
	return 0;
}