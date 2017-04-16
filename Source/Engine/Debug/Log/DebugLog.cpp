#include "DebugLog.h"

using namespace EngineAPI::Debug;

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
}