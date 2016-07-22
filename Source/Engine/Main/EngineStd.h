//EngineStd.h
//Created 22/07/16
//Created by Daniel Bowler
//
//Includes standard include files.

//Windows
#include <windows.h>
#include <windowsx.h>

//Dbg
#include <crtdbg.h>

//Entry for lib/engine in EngineMain.cpp. Projects using the engine lib should
//include this file so they can call the engines entry function. 
extern int APIENTRY EngineMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
