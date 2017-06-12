#include "VulkanRenderDevice.h"

//Manages (VK) queue families and queues
#include "../../CommandQueueFamily/CommandQueueFamily.h"

//Manages (VK) command buffer pools
#include "../../CommandBufferPool/CommandBufferPool.h"

//Manages memory allocations for us
#include "../../DeviceMemoryAllocator/DeviceMemoryAllocator.h"

using namespace EngineAPI::Graphics;
using namespace EngineAPI::Graphics::Platform;
//
//Device layers && extentions enabled. 
//
std::vector<const char*> deviceLayersNames =
{}; //Depreciated 

std::vector<const char *> deviceExtensionNames =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	//"VK_EXT_debug_report"
};

bool VulkanRenderDevice::OnResize(ESize2D newSize)
{
	//Done
	return true;
}

bool VulkanRenderDevice::InitVKPhysicalDevice(EngineAPI::OS::OSWindow* osWindow,
	EngineAPI::Graphics::RenderInstance* renderingInstance)
{
	//Get number of physical devices
	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(renderingInstance->GetVKInstance(), &physicalDeviceCount, nullptr);

	//Ensure that we have at least one valid devices
	if (physicalDeviceCount == 0)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderDevice::Init(): 0 VK Physical Devices found in this system.\n");
		return false;
	}
	else if (physicalDeviceCount > 1) //Just incase...
		EngineAPI::Debug::DebugLog::PrintWarningMessage("VulkanRenderDevice::Init(): > 1 VK Physical Devices found in this system!\n");

	//Get all the possible/available physical devices
	VkPhysicalDevice* vkPhysicalDevicesArray = GE_NEW VkPhysicalDevice[physicalDeviceCount];
	vkEnumeratePhysicalDevices(renderingInstance->GetVKInstance(), &physicalDeviceCount, &vkPhysicalDevicesArray[0]);

	//Pick the best...
	vkPhysicalDevice = PickBestVulkanPhysicalDevice(&vkPhysicalDevicesArray, physicalDeviceCount);

	//Cleanup memory holding all possible physical devices (we have cahced the handle of
	//our chosen device/GPU so this is fine)
	if (vkPhysicalDevicesArray)
		delete[] vkPhysicalDevicesArray;

	//Done
	return true;
}

bool VulkanRenderDevice::InitVKLogicalDeviceAndQueues(EngineAPI::OS::OSWindow* osWindow,
	EngineAPI::Graphics::RenderInstance* renderingInstance, const VkSurfaceKHR logicalSurfaceHandle)
{
	//Extension function pointers needed by the device - used to ask queues if they support presentation
	fpGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)vkGetInstanceProcAddr(renderingInstance->GetVKInstance(), "vkGetPhysicalDeviceSurfaceSupportKHR");
	if (!fpGetPhysicalDeviceSurfaceSupportKHR)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderDevice::InitVKLogicalDeviceAndQueues() - Could not find vkGetPhysicalDeviceSurfaceSupportKHR\n");
		return false;
	}

	//Validate device layers, extentions and feature set
	//
	//NOTE: Doesnt look for device extentions which are
	//provided by a layer -> Just finds the device extentions provided by the Vulkan
	//implementation
	if (!ValidateVKDeviceLayers(&deviceLayersNames))
		return false;
	if (!ValidateVKDeviceExtentions(renderingInstance->GetVKEnabledInstanceLayersList(), &deviceExtensionNames))
		return false;

	//Requested features
	vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &vkDeviceAvailableFeatures);

	vkDeviceEnabledFeaturesArray = {};
	if (vkDeviceAvailableFeatures.depthClamp == VK_TRUE)
		vkDeviceEnabledFeaturesArray.depthClamp = VK_TRUE;
	if (vkDeviceAvailableFeatures.samplerAnisotropy == VK_TRUE)
		vkDeviceEnabledFeaturesArray.samplerAnisotropy = VK_TRUE;

	//if (!ValidateVKDeviceFeatures(&vkDeviceEnabledFeaturesArray))
	//	return false;

	//********************************************************************************
	//********************************************************************************
	//***************************Queue(s) && familes**********************************
	//********************************************************************************
	//********************************************************************************
	//
	//Get the chosen devices queue families (count)
	vkQueueFamiliesCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &vkQueueFamiliesCount, nullptr); //Count
																								//Get all queue families
	vkQueueFamiliesArray = GE_NEW VkQueueFamilyProperties[vkQueueFamiliesCount];
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &vkQueueFamiliesCount, &vkQueueFamiliesArray[0]);

	//********************************************************************************
	//********************************************************************************
	//********************Graphics Queue && Presentation******************************
	//********************************************************************************
	//********************************************************************************
	//
	//Search queue families for the graphics queue: VK_QUEUE_GRAPHICS_BIT - We also want 
	//to find the queue with present abilities (ideally, the same queue family. However, we 
	//can adapt if it isnt)
	uint32_t graphicsQueueFamilyIdx = 0;
	uint32_t presentQueueFamilyIdx = 0;
	if (!GetGraphicsAndPresentQueueFamilyHandle(logicalSurfaceHandle, &vkQueueFamiliesArray[0], vkQueueFamiliesCount, &graphicsQueueFamilyIdx, &presentQueueFamilyIdx))
		return false;

	//Is both the graphics and present queues the same family?
	bool isSameQueueFamily = (graphicsQueueFamilyIdx == presentQueueFamilyIdx);
	
	//TEMP: Ensure that the graphics and presentation queues are the same
	if (!isSameQueueFamily)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderDevice::Init(): Graphics queue family and present queue family are not the same. Support for this is on the list of things to do!");
		return false;
	}

	//Graphics queue creation - returns the struct needed when creating the 
	//logical device. Note: Until we need a queue with support
	//for compute, we don't need to change VkDeviceQueueCreateInfo::pNext!
	VkQueueFlags queueFlag = vkQueueFamiliesArray[graphicsQueueFamilyIdx].queueFlags;

	VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {};
	float queuePriorities[1] = { 0.0 };
	graphicsQueueFamily.InitVKQueueFamily(&vkLogicalDevice,
		graphicsQueueFamilyIdx, queueFlag,
		ENGINE_CONFIG_VULKAN_API_GRAPHICS_QUEUE_COUNT, queuePriorities,
		&graphicsQueueCreateInfo);

	//********************************************************************************
	//********************************************************************************
	//**************************Create logical device*********************************
	//********************************************************************************
	//********************************************************************************
	//
	//Description struct
	VkDeviceCreateInfo vkDeviceCreateInfo = {};
	vkDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	vkDeviceCreateInfo.pNext = nullptr;
	vkDeviceCreateInfo.flags = 0; //Not used in VK 1.0.0
	vkDeviceCreateInfo.enabledLayerCount = (uint32_t)deviceLayersNames.size();								   //Depreciated
	vkDeviceCreateInfo.ppEnabledLayerNames = (deviceLayersNames.size() > 0) ? deviceLayersNames.data() : NULL;     //Depreciated
	vkDeviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensionNames.size();
	vkDeviceCreateInfo.ppEnabledExtensionNames = (deviceExtensionNames.size() > 0) ? deviceExtensionNames.data() : NULL;
	vkDeviceCreateInfo.pEnabledFeatures = &vkDeviceEnabledFeaturesArray;
	vkDeviceCreateInfo.pQueueCreateInfos = &graphicsQueueCreateInfo;     //Just graphics && presentation
	vkDeviceCreateInfo.queueCreateInfoCount = 1;					     //Just graphics && presentation

	//Create logical device
	VkResult result = vkCreateDevice(vkPhysicalDevice, &vkDeviceCreateInfo, nullptr, &vkLogicalDevice);

	//Validate that the logical device was created
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderDevice::Init(): vkCreateDevice() failed.\n");
		return false;
	}

	//Cache queue handle object(s) once logical device has been created
	if (!graphicsQueueFamily.InitVKQueues(&vkLogicalDevice))
		return false;

	//Done
	return true;
}

bool VulkanRenderDevice::InitVKMemoryAllocator(EngineAPI::OS::OSWindow* osWindow,
	EngineAPI::Graphics::RenderInstance* renderingInstance)
{
	//Cache memory info for the physical device
	CacheVKDeviceMemoryInfo();

	//TODO: Decide on best heap / types (by index) to use when allocating data
	//in specific ways for this device (eg: Different memoryType index will be used
	//during vkAlloc...() depending on if the memory should be mappable or not)
	uint32_t vkMemoryTypeIndexForEfficientDeviceOnlyAllocations = 0;
	if (!FindVKMemoryTypeIndexForProperties(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vkMemoryTypeIndexForEfficientDeviceOnlyAllocations))
		return false;

	uint32_t vkMemoryTypeIndexForMappableAllocations = 0; //TODO
	if (!FindVKMemoryTypeIndexForProperties(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT, &vkMemoryTypeIndexForMappableAllocations)) //TODO
		return false;

	uint32_t gpuHeapIdx = vkDeviceMemoryProperties.memoryTypes[vkMemoryTypeIndexForEfficientDeviceOnlyAllocations].heapIndex;
	VkDeviceSize maxVRAM = vkDeviceMemoryProperties.memoryHeaps[gpuHeapIdx].size;

	//Allocate the memory allocator/manager
	if (!deviceMemoryAllocator.InitVKMemoryAllocator((EngineAPI::Graphics::RenderDevice*)this))
		return false;

	//Done
	return true;
}

bool VulkanRenderDevice::InitVKCommandBufferPools(EngineAPI::OS::OSWindow* osWindow,
	EngineAPI::Graphics::RenderInstance* renderingInstance)
{
	//Create command pools for the graphics queue family
	for (int i = 0; i < ENGINE_CONFIG_VULKAN_API_GRAPHICS_COMMAND_BUFFER_POOLS_COUNT; i++)
	{
		if (!graphicsQueueFamily.CreateVKCommandBufferPool(&vkLogicalDevice,
			ENGINE_CONFIG_VULKAN_API_GRAPHICS_COMMAND_BUFFER_POOLS_ALLOW_BUFFER_RESETS,
			ENGINE_CONFIG_VULKAN_API_GRAPHICS_COMMAND_BUFFER_POOLS_IS_TRANSIENT))
		{
			//Error
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderDevice::InitVKCommandBufferPools() Error: Could not create Command Buffer Pool\n");
			return false;
		}
	}

	//Done
	return true;
}

void VulkanRenderDevice::Shutdown()
{
	//Cleanup memory manager, stores and blocks - any VK resource not yet destroyed
	//will now be - you may want to comment out the Shutdown() function call if you want
	//to find VK* objects that have not been correctly cleaned up by the application (your application 
	//*does* cleanup its resources... doesnt it!?). 
	deviceMemoryAllocator.Shutdown();

	//Queue families that we use.
	graphicsQueueFamily.Shutdown();

	//Queue families info
	if (vkQueueFamiliesArray)
		delete[]vkQueueFamiliesArray;

	//Logical device 
	vkDestroyDevice(vkLogicalDevice, nullptr);
	vkLogicalDevice = NULL;
	vkPhysicalDevice = NULL;
}

void VulkanRenderDevice::CacheVKDeviceMemoryInfo()
{
	//Stores the memory properties of our selected device
	vkDeviceMemoryProperties = {};
	vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice, &vkDeviceMemoryProperties);
}

bool VulkanRenderDevice::FindVKMemoryTypeIndexForProperties(VkMemoryPropertyFlags properties, uint32_t* memoryTypeIndexOut)
{
	for (int i = 0; i < vkDeviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((vkDeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			*memoryTypeIndexOut = i;
			return true;
		}
	}

	//Failed
	EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderDevice::FindVKMemoryTypeIndexForProperties() Error. Couldn't find a memory type that supports the required properties\n");
	return false;
}

bool VulkanRenderDevice::ValidateVKDeviceLayers(std::vector<const char*> *desiredDeviceLayers)
{
	//No need to check...
	if (desiredDeviceLayers->size() == 0)
	{
#if SHOULD_PRINT_GRAPHICS_INIT_INFO
		EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanRenderDevice: Vulkan enabling NO device layers\n");
#endif
		return true;
	}

	//Get the device layers count
	uint32_t deviceLayersCount = 0;
	vkEnumerateDeviceLayerProperties(vkPhysicalDevice, &deviceLayersCount, nullptr);
	VkLayerProperties* availDeviceLayers = nullptr;

	//Get all device layers
	availDeviceLayers = GE_NEW VkLayerProperties[deviceLayersCount];
	vkEnumerateDeviceLayerProperties(vkPhysicalDevice, &deviceLayersCount, &availDeviceLayers[0]);

	//Are all of our requested device extentions available for us to use? 
	int numDeviceLayersMatchedWithRequested = 0;
	for (int i = 0; i < (int)deviceLayersCount; i++)
	{
		//ith device extentions properties
		VkLayerProperties* deviceLayer = &availDeviceLayers[i];

		//Can we find each and every one of our requested device extentions (by name)
		for (int j = 0; j < (int)desiredDeviceLayers->size(); j++)
		{
			if (strcmp(deviceLayer->layerName, (*desiredDeviceLayers)[j]) == 0)
			{
#if SHOULD_PRINT_GRAPHICS_INIT_INFO
				EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanRenderDevice: Vulkan enabling device layer: ");
				EngineAPI::Debug::DebugLog::PrintMessage(deviceLayer->layerName);
				EngineAPI::Debug::DebugLog::PrintMessage("\n");
#endif

				//Found
				numDeviceLayersMatchedWithRequested++;
				break;
			}
		}
	}

	//Cleanup 
	if (availDeviceLayers)
		delete[] availDeviceLayers;

	//Do validation
	if (numDeviceLayersMatchedWithRequested == desiredDeviceLayers->size())
		return true;
	else
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderDevice: Error when validating Vulkan Device Layers! ");
		EngineAPI::Debug::DebugLog::PrintMessage("It's likely that at least one requested device layer is not avilable for you to use in this Vulkan implentation.\n");
		return false;
	}
}

bool VulkanRenderDevice::ValidateVKDeviceExtentions(std::vector<const char*> *enabledInstanceLayers,
	std::vector<const char*> *desiredDeviceExtentions)
{
	//No need to check...
	if (desiredDeviceExtentions->size() == 0)
	{
#if SHOULD_PRINT_GRAPHICS_INIT_INFO
		EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanRenderDevice: Vulkan enabling NO device extentions\n");
#endif
		return true;
	}

	//Get the device extentions count - just those provided by the Vulkan implementation. 
	uint32_t deviceExtentionsCount = 0;
	vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, NULL, &deviceExtentionsCount, nullptr);
	VkExtensionProperties* availDeviceExtentions = nullptr;

	//Get all device extentions - just those provided by the Vulkan implementation. 
	availDeviceExtentions = GE_NEW VkExtensionProperties[deviceExtentionsCount];
	vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, NULL, &deviceExtentionsCount, &availDeviceExtentions[0]);

	//Are all of our requested device extentions available for us to use? 
	int numDeviceExtentionsMatchedWithRequested = 0;
	for (int i = 0; i < (int)deviceExtentionsCount; i++)
	{
		//ith device extentions properties
		VkExtensionProperties* deviceExtention = &availDeviceExtentions[i];

		//Can we find each and every one of our requested device extentions (by name)
		for (int j = 0; j < (int)desiredDeviceExtentions->size(); j++)
		{
			if (strcmp(deviceExtention->extensionName, (*desiredDeviceExtentions)[j]) == 0)
			{
#if SHOULD_PRINT_GRAPHICS_INIT_INFO
				EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanRenderDevice: Vulkan enabling device extention: ");
				EngineAPI::Debug::DebugLog::PrintMessage(deviceExtention->extensionName);
				EngineAPI::Debug::DebugLog::PrintMessage("\n");
#endif

				//Found
				numDeviceExtentionsMatchedWithRequested++;
				break;
			}
		}
	}

	//Cleanup memory
	delete[] availDeviceExtentions;

	//Do validation
	if (numDeviceExtentionsMatchedWithRequested == desiredDeviceExtentions->size())
		return true;
	else
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderDevice: Error when validating Vulkan Device Extentions! ");
		EngineAPI::Debug::DebugLog::PrintMessage("It's likely that at least one requested device extention is not avilable for you to use in this Vulkan implentation.\n");
		return false;
	}
}

/*
bool VulkanRenderDevice::ValidateVKDeviceFeatures(VkPhysicalDeviceFeatures* requestedDeviceFeatures)
{
//TODO
return true;
}
*/



VkPhysicalDevice VulkanRenderDevice::PickBestVulkanPhysicalDevice(VkPhysicalDevice** availPhysicalDevices,
	uint32_t availPhysicalDevicesCount)
{
	//
	//NOTE: Will need improvements later - A potential solution would be to pick one
	//that the engine deems to be the best (discrete && with the most VRam. Falling back
	//to something else if this doesnt exist incase the user is running an integrated
	//GPU) but offer the user the ability to change the physical/logical device
	//at runtime (give them a list of all possible vulkan GPUs/devices in the options
	//menu)
	//
	VkPhysicalDevice vkPickedPhysicalDevice = NULL;

	//If only one is available, pick it
	if (availPhysicalDevicesCount == 1)
		vkPickedPhysicalDevice = *availPhysicalDevices[0];
	else
	{
		//Pick the best somehow. 
		for (int i = 0; i < availPhysicalDevicesCount; i++)
		{
			//ith device properties - eg: Is it a discrete GPU?
			VkPhysicalDeviceProperties deviceProperties{};
			vkGetPhysicalDeviceProperties((*availPhysicalDevices)[i], &deviceProperties);

			//TEMP: Look for the first discrete GPU. Should really cache these
			//and then pick the one with the most VRAM. Falling back to integrated GPU and, 
			//if this fails, pick whatever & tell the user to upgrade their wooden PC!
			if (deviceProperties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				vkPickedPhysicalDevice = (*availPhysicalDevices)[i];
				break;
			}
		}
	}

	//Store info on picked device
	vkDeviceProperties = {};
	vkGetPhysicalDeviceProperties(vkPickedPhysicalDevice, &vkDeviceProperties);

#if SHOULD_PRINT_GRAPHICS_INIT_INFO 
	//Info to print
	//
	//String
	char str[256];
	int major = 0;
	int minor = 0;
	int patch = 0;

	//Device name
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanRenderDevice: Vulkan Device Selected: ");
	EngineAPI::Debug::DebugLog::PrintMessage(vkDeviceProperties.deviceName);
	EngineAPI::Debug::DebugLog::PrintMessage("\n");

	//Device type
	sprintf(&str[0], "%i", vkDeviceProperties.deviceType);
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanRenderDevice: Vulkan Device Type (VkPhysicalDeviceType Enum): ");
	EngineAPI::Debug::DebugLog::PrintMessage(str);
	EngineAPI::Debug::DebugLog::PrintMessage("\n");

	//Limits - device bytes
	uint64_t deviceBytes = vkDeviceProperties.limits.sparseAddressSpaceSize;
	sprintf(&str[0], "%u", deviceBytes);
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanRenderDevice: Maximum Available Memory: ");
	EngineAPI::Debug::DebugLog::PrintMessage(str);
	EngineAPI::Debug::DebugLog::PrintMessage("\n");

	//Driver version
	sprintf(&str[0], "%i.%i", VK_VERSION_MAJOR(vkDeviceProperties.driverVersion),
		VK_VERSION_MINOR(vkDeviceProperties.driverVersion));
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanRenderDevice: Vulkan Device Driver Version: ");
	EngineAPI::Debug::DebugLog::PrintMessage(str);
	EngineAPI::Debug::DebugLog::PrintMessage("\n");

	//Device ID && Vendor ID
	sprintf(&str[0], "%i. VendorID: %i", vkDeviceProperties.deviceID, vkDeviceProperties.vendorID);
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanRenderDevice: Vulkan Device ID: ");
	EngineAPI::Debug::DebugLog::PrintMessage(str);
	EngineAPI::Debug::DebugLog::PrintMessage("\n");

	//API version
	sprintf(&str[0], "%i.%i.%i", VK_VERSION_MAJOR(vkDeviceProperties.apiVersion),
		VK_VERSION_MINOR(vkDeviceProperties.apiVersion),
		VK_VERSION_PATCH(vkDeviceProperties.apiVersion));
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanRenderDevice: Vulkan Device API Version: ");
	EngineAPI::Debug::DebugLog::PrintMessage(str);
	EngineAPI::Debug::DebugLog::PrintMessage("\n");
#endif

	//Done
	return vkPickedPhysicalDevice;
}

bool VulkanRenderDevice::GetGraphicsAndPresentQueueFamilyHandle(const VkSurfaceKHR logicalSurface,
	VkQueueFamilyProperties* deviceQueueFamiliesArray, uint32_t queueFamilyCount,
	uint32_t* graphicsQueueFamilyIndexOut, uint32_t* presentQueeuFamilyIndexOut)
{
	//Which queue families support presentation?
	VkBool32* supportsPresentation = GE_NEW VkBool32[queueFamilyCount];
	for (int i = 0; i < queueFamilyCount; i++)
		fpGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, logicalSurface, &supportsPresentation[i]);

	//Index of graphics queue family && index of presentation family
	uint32_t graphicsFamilyIdx = UINT32_MAX;
	uint32_t presentationFamilyIdx = UINT32_MAX;

	//Find queue family that supports both graphics work && presentation work. 
	for (int i = 0; i < queueFamilyCount; i++)
	{
		if (deviceQueueFamiliesArray[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			if (graphicsFamilyIdx == UINT32_MAX)
				graphicsFamilyIdx = i;

			if (supportsPresentation[i] == VK_TRUE) //Ideally want a queue that supports both. 
			{
				graphicsFamilyIdx = i;
				presentationFamilyIdx = i;
				break;
			}
		}
	}

	//Did we fail to find graphics queue somehow??
	if (graphicsFamilyIdx == UINT32_MAX)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderDevice::GetGraphicsAndPresentQueueFamilyHandle(): Failed to find the graphics queue family.\n");
		delete[] supportsPresentation;
		return false;
	}

	//If we didnt find a queue that supports both graphics and presentation, find 
	//separate queues for the roles
	if (presentationFamilyIdx == UINT32_MAX)
	{
		for (int i = 0; i < queueFamilyCount; i++)
		{
			if (supportsPresentation[i] == VK_TRUE)
			{
				presentationFamilyIdx = i;
				break;
			}
		}
	}

	//Did we fail to find a presentation queue
	if (presentationFamilyIdx == UINT32_MAX)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderDevice::GetGraphicsAndPresentQueueFamilyHandle(): Failed to find the presentation queue family.\n");
		delete[] supportsPresentation;
		return false;
	}

	//Cleanup before returning 
	delete[] supportsPresentation;

	//Set output
	*graphicsQueueFamilyIndexOut = graphicsFamilyIdx;
	*presentQueeuFamilyIndexOut = presentationFamilyIdx;

	//Done
	return true;
}