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

//Standard c types - ??? Cross platform?
#include <cinttypes>

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
typedef uint64_t EUINT_64;
typedef uint32_t EUINT_32;
typedef uint16_t EUINT_16;
#endif

struct ESize2D
{
public:
	EUINT_32 Width;
	EUINT_32 Height;
};

//Struct representing a colour
struct Uint8Colour
{
	Uint8Colour() {};
	Uint8Colour(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		R = r;
		G = g;
		B = b;
		A = a;
	};

	uint8_t R;
	uint8_t G;
	uint8_t B;
	uint8_t A;
};

struct UNorm32Colour
{
	UNorm32Colour() {};
	UNorm32Colour(float r, float g, float b, float a)
	{
		R = r;
		G = g;
		B = b;
		A = a;
	};

	float R;
	float G;
	float B;
	float A;
};


//Allows us to pass 0 for EFlags if we want
typedef EUINT_32 EFlag;

//Global macros
//
//Converts from mebibyte to bytes
#define MEB_TO_BYTES(meb) meb * (1024 * 1024)

//For debug builds, output extra info everytime we dynamically allocate memory
#if defined(_DEBUG) | defined(DEBUG)
#define GE_NEW new(_NORMAL_BLOCK,__FILE__, __LINE__)
#else
#define GE_NEW new
#endif

//TODO? Delete...
#if defined(_DEBUG) | defined(DEBUG)
#define GE_DELETE delete
#else
#define GE_DELETE delete
#endif

#if defined(_DEBUG) | defined(DEBUG)
#define GE_DELETE_ARRAY delete[]
#else
#define GE_DELETE_ARRAY delete[]
#endif

//Macro to disable/hide copy constructor and assignment operator
#define HIDE_COPY_ASSIGNMENT(c) private: c(const c &other); c& operator = (const c &other);

//Macro to disable constructor, destructor, assignment operator && destructor (statics classes)
#define MAKE_STATICS_CLASS_ONLY(c) private: c(); ~c(); c(const c &other); c& operator = (const c &other);

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