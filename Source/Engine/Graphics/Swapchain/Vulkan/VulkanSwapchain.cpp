#include "VulkanSwapchain.h"

using namespace EngineAPI::Graphics::Platform;

bool VulkanSwapchain::InitVKLogicalSurface(EngineAPI::OS::OSWindow* osWindow,
	EngineAPI::Graphics::RenderInstance* renderingInstance,
	EngineAPI::Graphics::RenderDevice* renderingDevice)
{
#ifdef ENGINE_CONFIG_PLATFORM_WIN32
	return InitWin32LogicalSurface(osWindow, renderingInstance, renderingDevice);
#endif
}

bool VulkanSwapchain::InitVKSwapchain(EngineAPI::OS::OSWindow* osWindow,
	EngineAPI::Graphics::RenderInstance* renderingInstance,
	EngineAPI::Graphics::RenderDevice* renderingDevice)
{
#ifdef ENGINE_CONFIG_PLATFORM_WIN32
	return InitWin32Swapchain(osWindow, renderingInstance, renderingDevice);
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
bool VulkanSwapchain::InitWin32LogicalSurface(EngineAPI::OS::OSWindow* osWindow,
	EngineAPI::Graphics::RenderInstance* renderingInstance,
	EngineAPI::Graphics::RenderDevice* renderingDevice)
{
	//Cache instance & device
	cachedVKInstance = renderingInstance->GetVKInstance();
	cachedVKDevice = renderingDevice->GetVKLogicalDevice();

	//Query for WSI extension function pointers
	//
	//Instance:
	//fpGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR) vkGetInstanceProcAddr(cachedVKInstance, "vkGetPhysicalDeviceSurfaceSupportKHR");
	fpGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)vkGetInstanceProcAddr(cachedVKInstance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
	fpGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)vkGetInstanceProcAddr(cachedVKInstance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
	fpGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)vkGetInstanceProcAddr(cachedVKInstance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
	fpDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)vkGetInstanceProcAddr(cachedVKInstance, "vkDestroySurfaceKHR");

	//Device:
	fpCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR) vkGetDeviceProcAddr(cachedVKDevice, "vkCreateSwapchainKHR");
	fpDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)vkGetDeviceProcAddr(cachedVKDevice, "vkDestroySwapchainKHR");
	fpGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)vkGetDeviceProcAddr(cachedVKDevice, "vkGetSwapchainImagesKHR");
	fpAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)vkGetDeviceProcAddr(cachedVKDevice, "vkAcquireNextImageKHR");
	fpQueuePresentKHR = (PFN_vkQueuePresentKHR)vkGetDeviceProcAddr(cachedVKDevice, "vkQueuePresentKHR");
	
	//Error check
	//
	//Instance:
	//if (!fpGetPhysicalDeviceSurfaceSupportKHR)
	//{
	//	EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::InitWin32(): fpGetPhysicalDeviceSurfaceSupportKHR is NULL\n");
	//	return false;
	//}
	if (!fpGetPhysicalDeviceSurfaceCapabilitiesKHR)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::InitWin32(): fpGetPhysicalDeviceSurfaceCapabilitiesKHR is NULL\n");
		return false;
	}
	if (!fpGetPhysicalDeviceSurfaceFormatsKHR)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::InitWin32(): fpGetPhysicalDeviceSurfaceFormatsKHR is NULL\n");
		return false;
	}
	if (!fpGetPhysicalDeviceSurfacePresentModesKHR)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::InitWin32(): fpGetPhysicalDeviceSurfacePresentModesKHR is NULL\n");
		return false;
	}
	if (!fpDestroySurfaceKHR)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::InitWin32(): fpDestroySurfaceKHR is NULL\n");
		return false;
	}
	
	//Device:
	if (!fpCreateSwapchainKHR)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::InitWin32(): fpCreateSwapchainKHR is NULL\n");
		return false;
	}
	if (!fpDestroySwapchainKHR)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::InitWin32(): fpDestroySwapchainKHR is NULL\n");
		return false;
	}
	if (!fpGetSwapchainImagesKHR)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::InitWin32(): fpGetSwapchainImagesKHR is NULL\n");
		return false;
	}
	if (!fpAcquireNextImageKHR)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::InitWin32(): fpAcquireNextImageKHR is NULL\n");
		return false;
	}
	if (!fpQueuePresentKHR)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::InitWin32(): fpQueuePresentKHR is NULL\n");
		return false;
	}

	//Create logical surface
	VkWin32SurfaceCreateInfoKHR logicalSurfaceCreateInfo = {};
	logicalSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	logicalSurfaceCreateInfo.pNext = nullptr;
	logicalSurfaceCreateInfo.flags = 0;
	logicalSurfaceCreateInfo.hinstance = osWindow->GetAppInstanceHandle();
	logicalSurfaceCreateInfo.hwnd = osWindow->GetAppMainWindowHandle();

	VkResult result = vkCreateWin32SurfaceKHR(cachedVKInstance, &logicalSurfaceCreateInfo, nullptr, &vkSurfaceHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::InitWin32(): vkCreateWin32SurfaceKHR failed!\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanSwapchain::InitWin32Swapchain(EngineAPI::OS::OSWindow* osWindow,
	EngineAPI::Graphics::RenderInstance* renderingInstance,
	EngineAPI::Graphics::RenderDevice* renderingDevice)
{
	//Done
	return true;
}

void VulkanSwapchain::ShutdownWin32()
{
	//vkDestroySurfaceKHR(cachedVKInstance, vkSurfaceHandle, nullptr);
	fpDestroySurfaceKHR(cachedVKInstance, vkSurfaceHandle, nullptr);
}
#endif