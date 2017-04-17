//EngineStd.h
//Created 22/07/16
//Created by Daniel Bowler
//
//Includes standard include files. Think of this as a globals.h file

#pragma once

#include "../Config/EngineConfig.h" //Build settings

//Windows
#ifdef ENGINE_CONFIG_PLATFORM_WIN32
#include <windows.h>
#include <windowsx.h>

//Dbg
#include <crtdbg.h>

//tChar
#include <tchar.h>
#endif

//Forward declare the base application class. 
namespace EngineAPI
{
	namespace Core
	{
		class Application;
	};
};

//Typedefs
#ifdef ENGINE_CONFIG_PLATFORM_WIN32
typedef TCHAR EWIDECHAR;
typedef char ECHAR;
typedef bool EBOOL;
typedef int EINT;
typedef float EFLOAT;
typedef double EDOUBLE;
#endif

#ifdef ENGINE_CONFIG_PLATFORM_ORBIS
typedef char EWIDECHAR; //TODO
typedef char ECHAR;
typedef bool EBOOL;
typedef int EINT;
typedef float EFLOAT;
typedef double EDOUBLE;
#endif

//For debug builds, output extra info everytime we dynamically allocate memory
#if defined(_DEBUG) | defined(DEBUG)
#define GE_NEW new(_NORMAL_BLOCK,__FILE__, __LINE__)
#else
#define GE_NEW new
#endif

//Macro to disable/hide copy constructor and assignment operator
#define HIDE_COPY_ASSIGNMENT(c) private: c(const c &other); c& operator = (const c &other);

//Define for namespacers
#define BEGIN_NAMESPACE(n) namespace n { 
#define END_NAMESPACE };

//Entry for lib/engine in EngineMain.cpp. Projects using the engine lib should
//include this file so they can call the engines entry function. 
#ifdef ENGINE_CONFIG_PLATFORM_WIN32
extern int EngineMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow,
	int appVersionMajor, int appVersionMinor, int appVersionPatch, 
	EngineAPI::Core::Application* gameAppInstance);
#endif

#ifdef ENGINE_CONFIG_PLATFORM_ORBIS
extern int EngineMain(int argc, char* argv[], 
	int appVersionMajor, int appVersionMinor, int appVersionPatch,
	EngineAPI::Core::Application* gameAppInstance);
#endif