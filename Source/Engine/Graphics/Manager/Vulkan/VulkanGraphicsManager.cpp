#include "VulkanGraphicsManager.h"

using namespace EngineAPI::Graphics::Platform;

bool VulkanGraphicsManager::InitSubsystem()
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager::InitSubsystem()\n");
	
	//Init Vulkan API

	return true;
}

bool VulkanGraphicsManager::ShutdownSubsystem()
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager::ShutdownSubsystem()\n");
	return true;
}