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

#if defined(DEBUG) | defined(_DEBUG)
	//Enable console for debugging purposes
	ShowConsole();
#endif

	//Ensure an app exists.
	if (gameAppInstance == NULL)
		OutputDebugStringW(L"EngineMain Error: gameAppInstance == NULL. Make sure to create a project specific Application instance.\n");
	 
	//Set app global pointer & init the application. 
	g_App = (EngineAPI::Base::Application*)gameAppInstance;
	if (!g_App->Init(hInstance, lpCmdLine, NULL, 960, 540))
		return -1;

	//Loop

	//Done 
	return 0;
}