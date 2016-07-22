//EngineMain.cpp
//Created 21/07/16
//Created By Daniel Bowler
//
//Entry point for the engine

#pragma once

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>

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
	//Enable console for debugging
	ShowConsole();

	//Entry point for engine. 
	printf("Engine::Main()\n");

	//TEMP:
	while (1){}

	//Done 
	return 0;
}