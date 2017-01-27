//EngineMain.cpp
//Created 21/07/16
//Created By Daniel Bowler
//
//Entry point for the engine

#pragma once

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "EngineStd.h"
#include "../Application/Application.h"

//Libs : D3D11
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "D3DCompiler.lib")
#pragma comment (lib, "dxgi.lib")
//#pragma comment (lib, "dxerr.lib")

void ShowConsole()
{
	AllocConsole();

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle_out, _O_TEXT);
	FILE* hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);

	*stdout = *hf_out;
	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	hCrt = _open_osfhandle((long)handle_in, _O_TEXT);
	FILE* hf_in = _fdopen(hCrt, "r");
	setvbuf(hf_in, NULL, _IONBF, 128);
	*stdin = *hf_in;
}
  
int APIENTRY EngineMain(HINSTANCE hInstance, 
	HINSTANCE hPrevInstance,
	LPWSTR     lpCmdLine,
	int       nCmdShow, 
	EngineAPI::Base::Application* gameAppInstance)
{    
	//Set up checks for memory leaks.   
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//Enable console for debugging purposes
	ShowConsole();

	//Ensure an app exists.  
	if (gameAppInstance == NULL)
	{
		printf("EngineMain Error: gameAppInstance == NULL. Make sure to create a project specific Application instance.\n");
		return -1;
	}
		 
	//Set app global pointer & init the base engine
	g_App = (EngineAPI::Base::Application*)gameAppInstance;
	if (!g_App->InitEngine(hInstance, lpCmdLine, NULL, 960, 540))  
	{
		printf("EngineMain Error: Engine did not init correctly.\n");
		return -1;
	}

	//Once we have inited the engine, we want to init the app that is using the
	//engine. This is done by overriding the InitApplication() function of the
	//Application class
	if (!g_App->InitApplication())
	{
		printf("EngineMain Error: Application did not init correctly.\n");
		return -1;
	}
	
	//Enter main game loop
	g_App->EnterGameLoop();

	//Once game loop exits, shutdown and cleanup the (TODO: Game &) underlying engine
	g_App->ShutdownEngine(); 
	return 0;
}