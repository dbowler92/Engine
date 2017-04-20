#include "VulkanSwapchain.h"

using namespace EngineAPI::Graphics::Platform;

bool VulkanSwapchain::Init(EngineAPI::OS::OSWindow* osWindow,
	EngineAPI::Graphics::RenderInstance* renderingInstance,
	EngineAPI::Graphics::RenderDevice* renderingDevice)
{
#ifdef ENGINE_CONFIG_PLATFORM_WIN32
	return InitWin32(osWindow, renderingInstance, renderingDevice);
#endif
}

void VulkanSwapchain::Shutdown()
{
#ifdef ENGINE_CONFIG_PLATFORM_WIN32
	ShutdownWin32();
#endif
}

//
//Win32
//
#ifdef ENGINE_CONFIG_PLATFORM_WIN32
bool VulkanSwapchain::InitWin32(EngineAPI::OS::OSWindow* osWindow,
	EngineAPI::Graphics::RenderInstance* renderingInstance,
	EngineAPI::Graphics::RenderDevice* renderingDevice)
{
	return true;
}

void VulkanSwapchain::ShutdownWin32()
{

}
#endif