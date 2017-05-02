#include "VulkanGraphicsManager.h"

#include <iostream>

using namespace EngineAPI::Graphics::Platform;

bool VulkanGraphicsManager::InitSubsystem(EngineAPI::OS::OSWindow* osWindow,
	ECHAR* appTitle, int appVersionMajor, int appVersionMinor, int appVersionPatch,
	unsigned screenWidth, unsigned screenHeight)
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager::InitSubsystem()\n");

#if SHOULD_PRINT_GRAPHICS_INIT_INFO
	char vkInfoMsg[256];

	//Engine version
	sprintf(&vkInfoMsg[0], "VulkanGraphicsManager: Game Engine Build: %s. Version: %i, %i, %i\n",
		ENGINE_NAME, ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, ENGINE_VERSION_PATCH);
	EngineAPI::Debug::DebugLog::PrintInfoMessage(vkInfoMsg);

	//App Version
	sprintf(&vkInfoMsg[0], "VulkanGraphicsManager: Application Build: %s. Version: %i, %i, %i\n",
		appTitle, appVersionMajor, appVersionMinor, appVersionPatch);
	EngineAPI::Debug::DebugLog::PrintInfoMessage(vkInfoMsg);

	//Minimum vulkan API required
	sprintf(&vkInfoMsg[0], "VulkanGraphicsManager: Minimum Vulkan API Supported: %i, %i, %i\n",
		ENGINE_CONFIG_MINIMUM_VULKAN_API_MAJOR, ENGINE_CONFIG_MINIMUM_VULKAN_API_MINOR, ENGINE_CONFIG_MINIMUM_VULKAN_API_PATCH);
	EngineAPI::Debug::DebugLog::PrintInfoMessage(vkInfoMsg);
#endif

	//Alloc
	renderingInstance = GE_NEW EngineAPI::Graphics::RenderInstance();
	renderingDevice = GE_NEW EngineAPI::Graphics::RenderDevice();
	renderingSwapchain = GE_NEW EngineAPI::Graphics::Swapchain();

	//
	//Init vulkan API
	//
	if (!renderingInstance->InitVKInstance(osWindow, appTitle, appVersionMajor, appVersionMinor, appVersionPatch))
		return false;

	if (!renderingDevice->InitVKPhysicalDevice(osWindow, renderingInstance))
		return false;

	if (!renderingSwapchain->InitVKLogicalSurface(osWindow, renderingInstance, renderingDevice))
		return false;

	if (!renderingDevice->InitVKLogicalDeviceAndQueues(osWindow, renderingInstance))
		return false;
	if (!renderingDevice->InitVKMemoryBlocks(osWindow, renderingInstance))
		return false;
	if (!renderingDevice->InitVKCommandBufferPools(osWindow, renderingInstance))
		return false;
	//if (!renderingDevice->Init(osWindow, renderingInstance))
	//	return false;
	if (!renderingSwapchain->InitVKSwapchain(osWindow, renderingInstance, renderingDevice))
		return false;

	//Done
	return true;
}

bool VulkanGraphicsManager::ShutdownSubsystem()
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager::ShutdownSubsystem()\n");

	//Cleanup vulkan (reverse order to creation)
	renderingSwapchain->Shutdown();
	renderingDevice->Shutdown();
	renderingInstance->Shutdown();

	//Cleanup memory
	if (renderingSwapchain)
		delete renderingSwapchain;
	if (renderingDevice)
		delete renderingDevice;
	if (renderingInstance)
		delete renderingInstance;

	//Done
	return true;
}