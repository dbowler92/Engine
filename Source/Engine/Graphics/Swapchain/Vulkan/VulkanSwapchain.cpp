#include "VulkanSwapchain.h"

using namespace EngineAPI::Graphics::Platform;

bool VulkanSwapchain::InitVKLogicalSurface(EngineAPI::OS::OSWindow* osWindow,
	EngineAPI::Graphics::RenderInstance* renderingInstance)
{
	//Cache instance
	cachedVKInstance = renderingInstance->GetVKInstance();

	//Query for WSI extension function pointers
	if (!CacheInstanceExtentionFunctions(cachedVKInstance))
		return false;

	//Create logical surface (Win32)
	VkWin32SurfaceCreateInfoKHR logicalSurfaceCreateInfo = {};
	logicalSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	logicalSurfaceCreateInfo.pNext = nullptr;
	logicalSurfaceCreateInfo.flags = 0;
	logicalSurfaceCreateInfo.hinstance = osWindow->GetAppInstanceHandle();
	logicalSurfaceCreateInfo.hwnd = osWindow->GetAppMainWindowHandle();

	VkResult result = vkCreateWin32SurfaceKHR(cachedVKInstance, &logicalSurfaceCreateInfo, nullptr, &vkSurfaceHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::InitVKLogicalSurface(): vkCreateWin32SurfaceKHR failed!\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanSwapchain::InitVKSwapchain(EngineAPI::OS::OSWindow* osWindow,
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
	if (!CacheSwapchainColourImages())
		return false;

	//8) Creates VkImageViewes of the swapchain images
	if (!CreateVKSwapchainColourImageViews())
		return false;

#if GRAPHICS_CONFIG_DO_CREATE_DEPTH_TEXTURE_ALONGSIDE_SWAPCHAIN
	//9) Create the depth buffer image that we will be using.
	if (!CreateDepthBuffer(renderingDevice))
		return false;
#endif

	//10) Create fence used to wait for vkGetNextSwapchainImageKHR()
	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = nullptr;
	fenceCreateInfo.flags = 0; //Created in unsignalled state
	if (vkCreateFence(renderingDevice->GetVKLogicalDevice(), &fenceCreateInfo, nullptr, &vkGetNextImageFence) != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::InitVKSwapchain(): Error creating vkGetNextImageFence\n");
		return false;
	}

	//Done
	return true;
}

void VulkanSwapchain::Shutdown(bool doShutdownLogicalSurface, bool doShutdownSwapchainObject)
{
	//Cleanup fence
	if (vkGetNextImageFence)
	{
		vkGetFenceStatus(cachedVKDevice, vkGetNextImageFence) != VK_SUCCESS;
		vkDestroyFence(cachedVKDevice, vkGetNextImageFence, nullptr);
		vkGetNextImageFence = VK_NULL_HANDLE;
	}

	//Cleanup framebuffers & render instance command buffers
	for (int i = 0; i < swapchainFramebuffers.size(); i++)
		swapchainFramebuffers[i].Shutdown();
	swapchainFramebuffers.clear();
	swapchainImageCommandBuffers.clear(); //Reset them???

	//Cleanup array data
	delete[] surfaceFormatsArray;
	delete[] presentationModesArray;

	//Destroy depth texture
	depthTexture.Shutdown();

	//Delete image views - we own these not the WSI extention (unlike the underlying image)
	for (int i = 0; i < vkSwapchainColourImagesCount; i++)
		vkDestroyImageView(cachedVKDevice, vkSwapchainColourImageViews[i], nullptr);
	delete[] vkSwapchainColourImageViews;

	//Delete array of handles to the swapchain images  - DO NOT DELETE the
	//actual images -> The extention owns them and will destroy them
	//for us
	delete[] vkSwapchainColourImages;

	//Destroy swapchain
	if (doShutdownSwapchainObject)
	{
		fpDestroySwapchainKHR(cachedVKDevice, vkSwapchainHandle, nullptr);
		vkSwapchainHandle = VK_NULL_HANDLE;
	}

	//logical surface - only when fully shutting down the swapchain. On OnResize() events,
	//we don't need to delete this since the OSwindow (windows handle etc) remains the same!
	if (doShutdownLogicalSurface)
	{
		//vkDestroySurfaceKHR(cachedVKInstance, vkSurfaceHandle, nullptr);
		fpDestroySurfaceKHR(cachedVKInstance, vkSurfaceHandle, nullptr);
		vkSurfaceHandle = VK_NULL_HANDLE;
	}
}

bool VulkanSwapchain::OnResize(EngineAPI::OS::OSWindow* osWindow,
	EngineAPI::Graphics::RenderInstance* renderingInstance,
	EngineAPI::Graphics::RenderDevice* renderingDevice)
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanSwapchain::OnResize()\n");

	this->Shutdown(false, true);
	if (!this->InitVKSwapchain(osWindow, renderingInstance, renderingDevice))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::OnResize() Error: Failed to re-init swapchain with new size\n");
		return false;
	}

	//Done
	return true;
}

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
	{
		vkSwapchainSurfaceFormat = VK_FORMAT_B8G8R8A8_UNORM; //VK_FORMAT_R8G8B8A8_UNORM;
		vkSwapchainColourBufferColourSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	}
	else
	{
		vkSwapchainSurfaceFormat = surfaceFormatsArray[0].format; //TODO: Pick the best - may not need to be honest. 
		//vkSwapchainColourBufferColourSpace = surfaceFormatsArray[0].colorSpace;
		vkSwapchainColourBufferColourSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	}

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

	//Are the dimensions the same as the osWindow? If not, spit out a warning (for now since we want to pause
	//on said warning). Later, this should be a simple message as the app should be able to handle this
	//potential issue. 
	if (vkSwapchainExtents.width != osWindow->GetWindowWidth())
		EngineAPI::Debug::DebugLog::PrintWarningMessage("VulkanSwapchain::SelectDefaultSwapchainImageExtents() - Swapchain images width != OS window width\n");
	if (vkSwapchainExtents.height != osWindow->GetWindowHeight())
		EngineAPI::Debug::DebugLog::PrintWarningMessage("VulkanSwapchain::SelectDefaultSwapchainImageExtents() - Swapchain images height != OS window height\n");

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
	swapchainCreateInfo.imageColorSpace		  = vkSwapchainColourBufferColourSpace;
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

bool VulkanSwapchain::CacheSwapchainColourImages()
{
	//Number of swapchain images
	VkResult result = fpGetSwapchainImagesKHR(cachedVKDevice, vkSwapchainHandle, &vkSwapchainColourImagesCount, nullptr);

	//Match desired buffer count we sent in at Init time? 
	//
	//Driver presumably can overrule us and provide more images if it decides (im thinking of
	//nvidia inspector and forcing tripple buffering)
	if (vkSwapchainColourImagesCount != vkSwapchainDesiredBuffersCount) 
	{
		//Keep this as a warning for now...
		EngineAPI::Debug::DebugLog::PrintWarningMessage("VulkanSwapchain::CacheSwapchainImages() - Swapchain buffer count && images returned does not match\n");
		return false;
	}

	//Get all of the images and store the handles
	vkSwapchainColourImages = GE_NEW VkImage[vkSwapchainColourImagesCount];
	result = fpGetSwapchainImagesKHR(cachedVKDevice, vkSwapchainHandle, &vkSwapchainColourImagesCount, &vkSwapchainColourImages[0]);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::CacheSwapchainImages() - Error when caching swapchain VkImage handles\n");
		return false;
	}
	
	//Done
	return true;
}

bool VulkanSwapchain::CreateVKSwapchainColourImageViews()
{
	//Alloc array of image views. 
	vkSwapchainColourImageViews = GE_NEW VkImageView[vkSwapchainColourImagesCount];

	//For each colour image
	for (int i = 0; i < vkSwapchainColourImagesCount; i++)
	{
		//Creation info
		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = nullptr;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.format = vkSwapchainSurfaceFormat; 
		imageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY };
		imageViewCreateInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
		imageViewCreateInfo.subresourceRange.levelCount     = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount     = 1;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.image = vkSwapchainColourImages[i];

		//Create a VkImageView to each colour buffer
		VkResult result = vkCreateImageView(cachedVKDevice, &imageViewCreateInfo, NULL, &vkSwapchainColourImageViews[i]);
		if (result != VK_SUCCESS)
		{
			//Error
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::CreateVKSwapchainColourImageViews(): Error creating image views to swapchain colour images\n");
			return false;
		}
	}

	//Done
	return true;
}

bool VulkanSwapchain::CreateDepthBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice)
{
	//Debug name
	depthTexture.SetResourceDebugName("Swapchain Depth Texture");

	//Size - same as colour buffers. 
	ESize2D depthTextureDimentions;
	depthTextureDimentions.Width = (EUINT_32)vkSwapchainExtents.width;
	depthTextureDimentions.Height = (EUINT_32)vkSwapchainExtents.height;

	//Usage
	DepthTextureUsageFlag usageFlag = 0; //No need for shader input
	
	//Store -> Let the allocator decide for us for now. 
	EngineAPI::Graphics::DeviceMemoryStore* memStoreToUse = nullptr;

	//Init depth texture
#if GRAPHICS_CONFIG_ENABLE_MSAA
	uint32_t msaaSamples = GRAPHICS_CONFIG_MSAA_SAMPLE_COUNT;
#else
	uint32_t msaaSamples = 1;
#endif
	if (!depthTexture.InitVKDepthTexture(renderingDevice, GRAPHICS_CONFIG_DEPTH_TEXTURE_FORMAT, depthTextureDimentions, usageFlag, msaaSamples))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::CreateDepthBuffer() - Error initing depth texture\n");
		return false;
	}

	//TODO: Use a custom store for this resource???
	EngineAPI::Graphics::DeviceMemoryStore* swapchainDepthTextureStore = nullptr;
	VkDeviceSize storeSize = depthTexture.GetResourceVKMemoryRequirments().size;

	//Alloc depth texture
	if (!depthTexture.AllocAndBindVKDepthTexture(renderingDevice, swapchainDepthTextureStore))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::CreateDepthBuffer() - Error allocating depth texture\n");
		return false;
	}

	//Init layout and views
	if (!depthTexture.InitVKDepthTextureLayoutAndViews(renderingDevice))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::CreateDepthBuffer() - Error creating layout and views\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanSwapchain::InitVKFramebuffers(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Graphics::RenderPass* renderPass)
{
	//One framebuffer per swapchain colour buffer
	swapchainFramebuffers.resize(GetSwapchainColourBufferCount());

	//Colour + depth (Depth buffer is shared between all colour buffers -> The front and
	//back buffer (and maybe the tripple buffer))
	VkImageView attachments[2];
	attachments[1] = depthTexture.GetVKDepthStencilImageView();

	//Init each framebuffer
	for (int i = 0; i < swapchainFramebuffers.size(); i++)
	{
		//Colour buffer
		attachments[0] = GetVKImageViewForColourBuffer(i);

		//Description
		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.pNext = NULL;
		framebufferCreateInfo.renderPass = renderPass->GetVKRenderPassHandle();
		framebufferCreateInfo.attachmentCount = 2; //Colour + depth
		framebufferCreateInfo.pAttachments = attachments;
		framebufferCreateInfo.width = GetSwapchainDimentions().width;
		framebufferCreateInfo.height = GetSwapchainDimentions().height;
		framebufferCreateInfo.layers = 1;

		//Init
		if (!swapchainFramebuffers[i].InitVKFramebuffer(renderingDevice, &framebufferCreateInfo))
		{
			//Error
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::InitVKFramebuffers() Error: Could not init framebuffer\n");
			return false;
		}
	}


	//Done
	return true;
}

bool VulkanSwapchain::InitVKSwapchainRenderPassInstanceCommandBuffers(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Graphics::RenderPass* renderPass, 
	UNorm32Colour swpachainClearColour, float swapchainDepthClearValue, uint32_t swapchainStencilClearValue)
{
	//if recalling (Eg: Due to changes to the clear colour info, we need to delete the old command
	//buffers)
	if (swapchainImageCommandBuffers.size() != 0)
	{
		for (int i = 0; i < swapchainImageCommandBuffers.size(); i++)
		{
			if (!EngineAPI::Statics::VulkanStatics::CommandBufferReset(&swapchainImageCommandBuffers[i], true))
			{
				EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::InitVKSwapchainRenderPassInstanceCommandBuffers() - Could not reset command buffers\n");
				return false;
			}
		}

		//Clear vector
		swapchainImageCommandBuffers.clear();
	}

	//1 per swapchain colour image
	swapchainImageCommandBuffers.resize(GetSwapchainColourBufferCount());
	for (int i = 0; i < swapchainImageCommandBuffers.size(); i++)
	{
		//Get command buffer
		if (!renderingDevice->GetGraphicsCommandQueueFamily()->GetCommandBufferPool(0).GetVKCommandBufferFromPool(true, &swapchainImageCommandBuffers[i]))
		{
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::InitVKSwapchainRenderPassInstanceCommandBuffers() - Could not get command buffer\n");
			return false;
		}

		//Bind command buffer
		assert(EngineAPI::Statics::VulkanStatics::CommandBufferBeginRecordingDefault(&swapchainImageCommandBuffers[i]));

		//Clear values
		VkClearValue clearValues[2];

		//Colour buffer
		clearValues[0].color.float32[0] = swpachainClearColour.R;
		clearValues[0].color.float32[1] = swpachainClearColour.G;
		clearValues[0].color.float32[2] = swpachainClearColour.B;
		clearValues[0].color.float32[3] = swpachainClearColour.A;

		//Depth stencil buffer
		clearValues[1].depthStencil.depth = swapchainDepthClearValue;
		clearValues[1].depthStencil.stencil = swapchainStencilClearValue;

		//Create render pass instance -> This will be used to record a 
		//begin render pass command in to this buffer
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.pNext = nullptr;
		renderPassInfo.renderPass = renderPass->GetVKRenderPassHandle();
		renderPassInfo.framebuffer = swapchainFramebuffers[i].GetVKFramebufferHandle();
		renderPassInfo.renderArea.offset.x = 0;
		renderPassInfo.renderArea.offset.y = 0;
		renderPassInfo.renderArea.extent.width = GetSwapchainDimentions().width;
		renderPassInfo.renderArea.extent.height = GetSwapchainDimentions().height;
		renderPassInfo.clearValueCount = 2;
		renderPassInfo.pClearValues = clearValues;
	
		//Start recording the render pass instance
		vkCmdBeginRenderPass(swapchainImageCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		//End of render pass instance recording
		vkCmdEndRenderPass(swapchainImageCommandBuffers[i]);

		//End reading in to this command buffer
		assert(EngineAPI::Statics::VulkanStatics::CommandBufferEndRecording(&swapchainImageCommandBuffers[i]));
	}


	//Done
	return true;
}

bool VulkanSwapchain::BindAndClearSwapchainBuffers(EngineAPI::Graphics::RenderDevice* renderingDevice)
{
	//Get frontbuffer to render in to.
	VkResult result = fpAcquireNextImageKHR(renderingDevice->GetVKLogicalDevice(), vkSwapchainHandle, UINT64_MAX, 
		VK_NULL_HANDLE, vkGetNextImageFence, &currentColourBufferIndex);

	//Wait on fence
	if (vkGetNextImageFence)
		vkGetFenceStatus(renderingDevice->GetVKLogicalDevice(), vkGetNextImageFence) != VK_SUCCESS;

	//Bind and clear
	renderingDevice->GetGraphicsCommandQueueFamily()->SubmitVKCommandBuffersToQueueDefault(0,
		&swapchainImageCommandBuffers[currentColourBufferIndex], 1, VK_NULL_HANDLE, true);

	//Reset fence
	assert(vkResetFences(renderingDevice->GetVKLogicalDevice(), 1, &vkGetNextImageFence) == VK_SUCCESS);

	//Done
	return true;
}

bool VulkanSwapchain::Present(EngineAPI::Graphics::RenderDevice* renderingDevice)
{
	//Presentation queue
	EngineAPI::Graphics::CommandQueue& cmdQueue = renderingDevice->GetPresentCommandQueueFamily()->GetCommandQueues()[0];
	VkQueue presentQueue = cmdQueue.GetVKQueueHandle();
	
	//Presentation engine description
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &vkSwapchainHandle;
	presentInfo.pImageIndices = &currentColourBufferIndex;
	presentInfo.pResults = nullptr;

	//Queue image for presentation
	VkResult result = fpQueuePresentKHR(presentQueue, &presentInfo);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSwapchain::Present() Error: Could not present swapchain\n");
		return false;
	}

	//Done
	return true;
}