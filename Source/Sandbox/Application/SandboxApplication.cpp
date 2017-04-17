#include "SandboxApplication.h"

#include <iostream> //printf()
#include "../App Config/AppConfig.h"

SandboxApplication::SandboxApplication()
{}

ECHAR* SandboxApplication::GetGameTitle()
{
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
	return "Sandbox (Vulkan)"; 
#endif
#ifdef ENGINE_CONFIG_GRAPHICS_API_D3D11
	return "Sandbox (D3D11)";
#endif
#ifdef ENGINE_CONFIG_GRAPHICS_API_D3D12
	return "Sandbox (D3D12)";
#endif
#ifdef ENGINE_CONFIG_GRAPHICS_API_OPENGL
	return "Sandbox (OpenGL 4.x)";
#endif
#ifdef ENGINE_CONFIG_GRAPHICS_API_GNM
	return "Sandbox (GNM - Orbis)";
#endif
};

bool SandboxApplication::InitApplication()
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("SandboxApplication::InitApplication()\n");

	return true;
}

bool SandboxApplication::ShutdownApplication()
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("SandboxApplication::ShutdownApplication()\n");
	return true;
}