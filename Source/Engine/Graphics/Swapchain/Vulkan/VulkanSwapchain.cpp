#include "VulkanSwapchain.h"

using namespace EngineAPI::Graphics::Platform;

bool VulkanSwapchain::InitVKLogicalSurface(EngineAPI::OS::OSWindow* osWindow,
	EngineAPI::Graphics::RenderInstance* renderingInstance)
{
#ifdef ENGINE_CONFIG_PLATFORM_WIN32
	return InitWin32LogicalSurface(osWindow, renderingInstance);
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
	EngineAPI::Graphics::RenderInstance* renderingInstance)
{
	//Cache instance
	cachedVKInstance = renderingInstance->GetVKInstance();

	//Query for WSI extension function pointers
	//
	//Instance:
	//fpGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR) vkGetInstanceProcAddr(cachedVKInstance, "vkGetPhysicalDeviceSurfaceSupportKHR");
	fpGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)vkGetInstanceProcAddr(cachedVKInstance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
	fpGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)vkGetInstanceProcAddr(cachedVKInstance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
	fpGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)vkGetInstanceProcAddr(cachedVKInstance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
	fpDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)vkGetInstanceProcAddr(cachedVKInstance, "vkDestroySurfaceKHR");
	
	//Error check:
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
	//Cache logical device now it has been created.
	cachedVKDevice = renderingDevice->GetVKLogicalDevice();

	//Device:
	fpCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)vkGetDeviceProcAddr(cachedVKDevice, "vkCreateSwapchainKHR");
	fpDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)vkGetDeviceProcAddr(cachedVKDevice, "vkDestroySwapchainKHR");
	fpGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)vkGetDeviceProcAddr(cachedVKDevice, "vkGetSwapchainImagesKHR");
	fpAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)vkGetDeviceProcAddr(cachedVKDevice, "vkAcquireNextImageKHR");
	fpQueuePresentKHR = (PFN_vkQueuePresentKHR)vkGetDeviceProcAddr(cachedVKDevice, "vkQueuePresentKHR");

	//Error check:
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

	//Get supported formats for the swapchain
	uint32_t surfaceFormatsCount = 0;
	VkSurfaceFormatKHR* surfaceFormatsArray = nullptr;
	VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(renderingDevice->GetVKPhysicalDevice(), vkSurfaceHandle, &surfaceFormatsCount, nullptr);
	
	//Get all formats
	surfaceFormatsArray = GE_NEW VkSurfaceFormatKHR[surfaceFormatsCount];
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(renderingDevice->GetVKPhysicalDevice(), vkSurfaceHandle, &surfaceFormatsCount, &surfaceFormatsArray[0]);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::InitWin32Swapchain() Error: Could not get surface formats\n");
		return false;
	}

	//Select a format
	if (!SelectVKSurfaceFormatForSwapchain(surfaceFormatsArray, surfaceFormatsCount, &vkSurfaceFormat))
		return false;

	//Cleanup before returning
	delete[] surfaceFormatsArray;

	//Done
	return true;
}

void VulkanSwapchain::ShutdownWin32()
{
	//vkDestroySurfaceKHR(cachedVKInstance, vkSurfaceHandle, nullptr);
	fpDestroySurfaceKHR(cachedVKInstance, vkSurfaceHandle, nullptr);
}
#endif

bool VulkanSwapchain::SelectVKSurfaceFormatForSwapchain(VkSurfaceFormatKHR* surfaceFormatsArray, uint32_t surfaceFormatsCount,
	VkFormat* surfaceFormatOut)
{
	//Incase VK_FORMAT_UNDEFINED, use default R8G8B8A8_UNORM format
	if (surfaceFormatsCount == 1 && surfaceFormatsArray[0].format == VK_FORMAT_UNDEFINED)
		*surfaceFormatOut = VK_FORMAT_R8G8B8A8_UNORM;
	else
		*surfaceFormatOut = surfaceFormatsArray[0].format; //TODO: Pick the best - may not need to be honest. 

	//Done
	return true;
}
