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
	//Cleanup array data
	delete[] surfaceFormatsArray;
	delete[] presentationModesArray;

	//Delete array of handles to the swapchain images - DO NOT DELETE the
	//actual images -> The extention owns them and will destroy them
	//for us
	delete[] vkSwapchainImages;

	//Destroy swapchain
	fpDestroySwapchainKHR(cachedVKDevice, vkSwapchainHandle, nullptr);
	vkSwapchainHandle = VK_NULL_HANDLE;

	//vkDestroySurfaceKHR(cachedVKInstance, vkSurfaceHandle, nullptr);
	fpDestroySurfaceKHR(cachedVKInstance, vkSurfaceHandle, nullptr);
	vkSurfaceHandle = VK_NULL_HANDLE;
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
	if (!CacheInstanceExtentionFunctions(cachedVKInstance))
		return false;
	
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

	//Physical device we are using
	VkPhysicalDevice physicalDeviceHandle = renderingDevice->GetVKPhysicalDevice();

	//Query extension functions (device)
	if (!CacheDeviceExtentionFunctions(cachedVKDevice))
		return false;

	//Get all supported formats
	if (!CacheSurfaceFormats(physicalDeviceHandle))
		return false;

	//Get surface capabilities for surface image creation
	if (!CacheSurfaceCapabilities(physicalDeviceHandle))
		return false;

	//Presentation modes for surface image creation
	if (!CacheSurfacePresentationModes(physicalDeviceHandle))
		return false;

	//
	//Swapchain setup (setup the swapchain based on default settings - need to have a system
	//that changes these settings based on a) .ini file and b) runtime changes from the
	//user - eg: Enabling/disabling vsync will need to change the presentation mode)
	//
	//1) Select a surface format
	if (!SelectDefaultVKSurfaceFormatForSwapchain())
		return false;

	//2) Size of images
	if (!SelectDefaultSwapchainImageExtents(osWindow))
		return false;

	//3) number of buffers
	if (!SelectDefaultVKSwapchainImageCount())
		return false;
	
	//4) Presentation mode/state
	if (!SelectDefaultVKPresentationMode())
		return false;
	
	//5) Pre transform
	if (!SelectDefaultVKPreTransform())
		return false;

	//6) Create the vkSwapchain object - based on default settings
	if (!CreateVKSwpachain())
		return false;

	//7) Gets a reference to the swapchain images
	if (!CacheSwapchainImages())
		return false;

	//Done
	return true;
}
#endif

bool VulkanSwapchain::CacheInstanceExtentionFunctions(VkInstance instance)
{
	//Query for WSI extension function pointers
	//
	//fpGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR) vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
	fpGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
	fpGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
	fpGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
	fpDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR");

	//Error check:
	//if (!fpGetPhysicalDeviceSurfaceSupportKHR)
	//{
	//	EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::CacheInstanceExtentionFunctions(): fpGetPhysicalDeviceSurfaceSupportKHR is NULL\n");
	//	return false;
	//}
	if (!fpGetPhysicalDeviceSurfaceCapabilitiesKHR)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::CacheInstanceExtentionFunctions(): fpGetPhysicalDeviceSurfaceCapabilitiesKHR is NULL\n");
		return false;
	}
	if (!fpGetPhysicalDeviceSurfaceFormatsKHR)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::CacheInstanceExtentionFunctions(): fpGetPhysicalDeviceSurfaceFormatsKHR is NULL\n");
		return false;
	}
	if (!fpGetPhysicalDeviceSurfacePresentModesKHR)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::CacheInstanceExtentionFunctions(): fpGetPhysicalDeviceSurfacePresentModesKHR is NULL\n");
		return false;
	}
	if (!fpDestroySurfaceKHR)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::CacheInstanceExtentionFunctions(): fpDestroySurfaceKHR is NULL\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanSwapchain::CacheDeviceExtentionFunctions(VkDevice device)
{
	fpCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)vkGetDeviceProcAddr(device, "vkCreateSwapchainKHR");
	fpDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)vkGetDeviceProcAddr(device, "vkDestroySwapchainKHR");
	fpGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)vkGetDeviceProcAddr(device, "vkGetSwapchainImagesKHR");
	fpAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)vkGetDeviceProcAddr(device, "vkAcquireNextImageKHR");
	fpQueuePresentKHR = (PFN_vkQueuePresentKHR)vkGetDeviceProcAddr(device, "vkQueuePresentKHR");

	//Error check:
	if (!fpCreateSwapchainKHR)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::CacheDeviceExtentionFunctions(): fpCreateSwapchainKHR is NULL\n");
		return false;
	}
	if (!fpDestroySwapchainKHR)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::CacheDeviceExtentionFunctions(): fpDestroySwapchainKHR is NULL\n");
		return false;
	}
	if (!fpGetSwapchainImagesKHR)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::CacheDeviceExtentionFunctions(): fpGetSwapchainImagesKHR is NULL\n");
		return false;
	}
	if (!fpAcquireNextImageKHR)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::CacheDeviceExtentionFunctions(): fpAcquireNextImageKHR is NULL\n");
		return false;
	}
	if (!fpQueuePresentKHR)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::CacheDeviceExtentionFunctions(): fpQueuePresentKHR is NULL\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanSwapchain::CacheSurfaceFormats(VkPhysicalDevice physicalDeviceHandle)
{
	//Get supported formats for the swapchain
	VkResult result = fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDeviceHandle, vkSurfaceHandle, &surfaceFormatsCount, nullptr);

	//Get all formats
	surfaceFormatsArray = GE_NEW VkSurfaceFormatKHR[surfaceFormatsCount];
	result = fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDeviceHandle, vkSurfaceHandle, &surfaceFormatsCount, &surfaceFormatsArray[0]);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::CacheSurfaceFormats() Error: Could not get surface formats\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanSwapchain::CacheSurfaceCapabilities(VkPhysicalDevice physicalDeviceHandle)
{
	//Get surface capabilities for surface image creation
	VkResult result = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDeviceHandle, vkSurfaceHandle, &surfaceCapabilities);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::CacheSurfaceCapabilities() Error: Could not get surface capabilities\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanSwapchain::CacheSurfacePresentationModes(VkPhysicalDevice physicalDeviceHandle)
{
	//Presentation modes for surface image creation
	VkResult result = fpGetPhysicalDeviceSurfacePresentModesKHR(physicalDeviceHandle, vkSurfaceHandle, &presentationModesCount, nullptr);

	//Get all supported presentation modes
	presentationModesArray = GE_NEW VkPresentModeKHR[presentationModesCount];
	result = fpGetPhysicalDeviceSurfacePresentModesKHR(physicalDeviceHandle, vkSurfaceHandle, &presentationModesCount, &presentationModesArray[0]);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::InitWin32Swapchain() Error: Could not get surface presentation modes\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanSwapchain::SelectDefaultVKSurfaceFormatForSwapchain()
{
	//Incase VK_FORMAT_UNDEFINED, use default R8G8B8A8_UNORM format
	if (surfaceFormatsCount == 1 && surfaceFormatsArray[0].format == VK_FORMAT_UNDEFINED)
		vkSwapchainSurfaceFormat = VK_FORMAT_R8G8B8A8_UNORM;
	else
		vkSwapchainSurfaceFormat = surfaceFormatsArray[0].format; //TODO: Pick the best - may not need to be honest. 

	//Done
	return true;
}

bool VulkanSwapchain::SelectDefaultSwapchainImageExtents(EngineAPI::OS::OSWindow* osWindow)
{
	//Note: If the surface width and height are not defined, set them
	//to match the size of the OS window
	if (surfaceCapabilities.currentExtent.width == (uint32_t)-1)
	{
		vkSwapchainExtents.width = osWindow->GetWindowWidth();
		vkSwapchainExtents.height = osWindow->GetWindowHeight();
	}
	else
	{
		//Surface width and height are set - swapchain must match this
		//size.
		vkSwapchainExtents = surfaceCapabilities.currentExtent;
	}

	//Done 
	return true;
}

bool VulkanSwapchain::SelectDefaultVKPresentationMode()
{
	if (presentationModesCount == 0)
	{
		//Big error...
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::SelectVKPresentationMode() - Error: No present modes available for this surface!\n");
		return false;
	}

	//Does desired mode work
	bool isUsingDesired = false;
	bool hasFoundAnyMode = false;
	for (int i = 0; i < presentationModesCount; i++)
	{
		//Desired
		if (presentationModesArray[i] == ENGINE_CONFIG_VULKAN_API_SWAPCHAIN_DEFAULT_PRESENT_MODE)
		{
			vkSwapchainPresentMode = ENGINE_CONFIG_VULKAN_API_SWAPCHAIN_DEFAULT_PRESENT_MODE;
			isUsingDesired = true;
			hasFoundAnyMode = true;
			break;
		}
	}

	if (!isUsingDesired)
	{
		//Backups...
		VkPresentModeKHR backupPresentModes[4]; //in order...
		backupPresentModes[0] = VK_PRESENT_MODE_IMMEDIATE_KHR;
		backupPresentModes[1] = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
		backupPresentModes[2] = VK_PRESENT_MODE_FIFO_KHR;
		backupPresentModes[3] = VK_PRESENT_MODE_MAILBOX_KHR;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < presentationModesCount; j++)
			{
				if (backupPresentModes[i] == presentationModesArray[j])
				{
					vkSwapchainPresentMode = backupPresentModes[i];
					isUsingDesired = false;
					hasFoundAnyMode = true;
					break; //Finds the first supported in the backups array then is done. 
				}
			}
		}
	}

	//No modes found somehow...
	if (!hasFoundAnyMode)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::SelectVKPresentationMode() - Could not find desired OR backup present modes\n");
		return false;
	}

	//Desired present mode isn't supported. However, we found an alternative - print warning. 
	if (!isUsingDesired)
		EngineAPI::Debug::DebugLog::PrintWarningMessage("VulkanSwapchain::SelectVKPresentationMode() - Desired mode not supported. Defaulting to setting\n");

	//Done. 
	return true;
}

bool VulkanSwapchain::SelectDefaultVKSwapchainImageCount()
{
	//Ensure that surface can support at least 1 - Should never happen...
	if (surfaceCapabilities.maxImageCount == 0)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::SelectDefaultVKSwapchainImageCount() - Error: Surface supports 0 images in swapchain\n");
		return false;
	}

	//Ensure that desired is > 0 - dump typo that you can fix. 
	if (ENGINE_CONFIG_VULKAN_API_SWAPCHAIN_DEFAULT_BUFFER_COUNT == 0)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::SelectDefaultVKSwapchainImageCount() - Error: Desired buffer count <= 0\n");
		return false;
	}

	if (ENGINE_CONFIG_VULKAN_API_SWAPCHAIN_DEFAULT_BUFFER_COUNT <= surfaceCapabilities.maxImageCount)
	{
		//Desired buffer count is fine
		vkSwapchainDesiredBuffersCount = ENGINE_CONFIG_VULKAN_API_SWAPCHAIN_DEFAULT_BUFFER_COUNT;
		return true;
	}
	else
	{
		//Default to max - print warning
		vkSwapchainDesiredBuffersCount = surfaceCapabilities.maxImageCount;
		EngineAPI::Debug::DebugLog::PrintWarningMessage("VulkanSwapchain::SelectDefaultVKSwapchainImageCount() - Desired buffer count not supported. Defaulting to max\n");
		return true; 
	}	
}

bool VulkanSwapchain::SelectDefaultVKPreTransform()
{
	if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) 
		vkSwapchainSurfacePreTransformFlags = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	else
		vkSwapchainSurfacePreTransformFlags = surfaceCapabilities.currentTransform;

	//Done
	return true;
}

bool VulkanSwapchain::CreateVKSwpachain()
{
	//Destroy old?
	//
	//TODO: Is this the right way of doing it???? See: swapchainCreateInfo.oldSwapchain
	if (vkSwapchainHandle != VK_NULL_HANDLE)
	{
		EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanSwapchain::CreateVKSwpachain(): Destroying old swapchain.\n");
		fpDestroySwapchainKHR(cachedVKDevice, vkSwapchainHandle, nullptr);
		vkSwapchainHandle = VK_NULL_HANDLE;
	}

	//Alloc info for the new or initial swapchain
	VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
	swapchainCreateInfo.sType				  = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.pNext				  = nullptr;
	swapchainCreateInfo.flags				  = 0;
	swapchainCreateInfo.surface				  = vkSurfaceHandle;
	swapchainCreateInfo.minImageCount		  = vkSwapchainDesiredBuffersCount;
	swapchainCreateInfo.imageFormat			  = vkSwapchainSurfaceFormat;
	swapchainCreateInfo.imageExtent.width	  = vkSwapchainExtents.width;
	swapchainCreateInfo.imageExtent.height	  = vkSwapchainExtents.height;
	swapchainCreateInfo.preTransform		  = vkSwapchainSurfacePreTransformFlags;
	swapchainCreateInfo.compositeAlpha		  = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.imageArrayLayers	  = 1;
	swapchainCreateInfo.presentMode			  = vkSwapchainPresentMode;
	swapchainCreateInfo.oldSwapchain		  = VK_NULL_HANDLE;  //???Incase of recreating, should this be vkSwapchainHandle && not destroyed manually above???
	swapchainCreateInfo.clipped				  = true;
	swapchainCreateInfo.imageColorSpace		  = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	swapchainCreateInfo.imageUsage		      = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	swapchainCreateInfo.imageSharingMode	  = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCreateInfo.queueFamilyIndexCount = 0;
	swapchainCreateInfo.pQueueFamilyIndices	  = nullptr;

	//Create it. 
	VkResult result = fpCreateSwapchainKHR(cachedVKDevice, &swapchainCreateInfo, nullptr, &vkSwapchainHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::CreateVKSwpachain() - Error: Could not create VkSwapchain\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanSwapchain::CacheSwapchainImages()
{
	//Number of swapchain images
	VkResult result = fpGetSwapchainImagesKHR(cachedVKDevice, vkSwapchainHandle, &vkSwapchainImagesCount, nullptr);

	//Match desired buffer count we sent in at Init time? 
	//
	//Driver presumably can overrule us and provide more images if it decides (im thinking of
	//nvidia inspector and forcing tripple buffering)
	if (vkSwapchainImagesCount != vkSwapchainDesiredBuffersCount)
	{
		//Keep this as a warning for now...
		EngineAPI::Debug::DebugLog::PrintWarningMessage("VulkanSwapchain::CacheSwapchainImages() - Swapchain buffer count && images returned does not match\n");
		return false;
	}

	//Get all of the images and store the handles
	vkSwapchainImages = GE_NEW VkImage[vkSwapchainImagesCount];
	result = fpGetSwapchainImagesKHR(cachedVKDevice, vkSwapchainHandle, &vkSwapchainImagesCount, &vkSwapchainImages[0]);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::CacheSwapchainImages() - Error when caching swapchain VkImage handles\n");
		return false;
	}
	
	//Done
	return true;
}