#include "D3D11GraphicsManager.h"
#include <iostream>

using namespace EngineAPI::Graphics::Platform;

bool D3D11GraphicsManager::InitSubsystem(EngineAPI::OS::OSWindow* osWindow,
	ECHAR* appTitle, int appVersionMajor, int appVersionMinor, int appVersionPatch,
	unsigned screenWidth, unsigned screenHeight)
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("D3D11GraphicsManager::InitSubsystem()\n");

#if SHOULD_PRINT_GRAPHICS_INIT_INFO
	//Debug string
	char d3d11InfoMsg[256];
#endif

	return true;
}

bool D3D11GraphicsManager::ShutdownSubsystem()
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("D3D11GraphicsManager::ShutdownSubsystem()\n");
	return true;
}