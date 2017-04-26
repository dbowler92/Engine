#include "DebugLog.h"

using namespace EngineAPI::Debug;

void DebugLog::PrintMessage(const ECHAR* msg)
{
#ifdef ENGINE_CONFIG_PLATFORM_WIN32
	OutputDebugStringA(msg);
#endif
#ifdef ENGINE_CONFIG_PLATFORM_ORBIS
	printf(msg);
#endif
}

void DebugLog::PrintInfoMessage(const ECHAR* msg)
{
#ifdef ENGINE_CONFIG_PLATFORM_WIN32
	OutputDebugStringA("Debug Info: ");
	OutputDebugStringA(msg);
#endif
#ifdef ENGINE_CONFIG_PLATFORM_ORBIS
	printf("Debug Info: ");
	printf(msg);
#endif
}

void DebugLog::PrintWarningMessage(const ECHAR* msg)
{
#ifdef ENGINE_CONFIG_PLATFORM_WIN32 
	OutputDebugStringA("Debug Warning: ");
	OutputDebugStringA(msg);
#endif
#ifdef ENGINE_CONFIG_PLATFORM_ORBIS
	printf("Debug Warning: ");
	printf(msg);
#endif

#if ENGINE_CONFIG_STOP_EXECUTION_ON_WARNING_MSG
	PrintInfoMessage("DebugLog: Halting execution on warning...\n");
	while (1) {};
#endif
}

void DebugLog::PrintErrorMessage(const ECHAR* msg)
{
#ifdef ENGINE_CONFIG_PLATFORM_WIN32 
	OutputDebugStringA("Debug Error: ");
	OutputDebugStringA(msg);
#endif
#ifdef ENGINE_CONFIG_PLATFORM_ORBIS
	printf("Debug Error: ");
	printf(msg);
#endif

#if ENGINE_CONFIG_STOP_EXECUTION_ON_ERROR_MSG
	PrintInfoMessage("DebugLog: Halting execution on error...\n");
	while (1) {};
#endif
}