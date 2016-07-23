//EngineStd.h
//Created 22/07/16
//Created by Daniel Bowler
//
//Includes standard include files. Think of this as a globals.h file

#pragma once

//Windows
#include <windows.h>
#include <windowsx.h>

//Dbg
#include <crtdbg.h>

//Forward declare the base application class. 
namespace EngineAPI
{
	class Application;
};

//For debug builds, output extra info everytime we dynamically allocate memory
#if defined(_DEBUG) | defined(DEBUG)
	#define GE_NEW new(_NORMAL_BLOCK,__FILE__, __LINE__)
#else
	#define GE_NEW new
#endif

//Macro to disable/hide copy constructor and assignment operator
#define HIDE_COPY_ASSIGNMENT(c) private: c(const c &other); c& operator = (const c &other);

//Entry for lib/engine in EngineMain.cpp. Projects using the engine lib should
//include this file so they can call the engines entry function. 
extern int APIENTRY EngineMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, int nCmdShow, EngineAPI::Application* gameAppInstance);
