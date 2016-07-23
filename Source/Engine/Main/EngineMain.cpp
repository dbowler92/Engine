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
	LPSTR     lpCmdLine,
	int       nCmdShow)
{ 
	//Set up checks for memory leaks.  
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//Enable console for debugging purposes
	ShowConsole();

	//Init engine

	//Done 
	return 0;
}