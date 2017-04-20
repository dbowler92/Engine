#include "VulkanSwapchain.h"

using namespace EngineAPI::Graphics::Platform;

bool VulkanSwapchain::Init(EngineAPI::OS::OSWindow* osWindow,
	EngineAPI::Graphics::RenderInstance* renderingInstance,
	EngineAPI::Graphics::RenderDevice* renderingDevice)
{
	//
	//Win32
	//
	VkWin32SurfaceCreateInfoKHR win32SurfaceInfo = {};

	//vkCreateWin32SurfaceKHR()

	//Done 
	return true;
}

void VulkanSwapchain::Shutdown()
{
	//TODO
}