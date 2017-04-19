#include "VulkanGraphicsManager.h"

#include <iostream>

using namespace EngineAPI::Graphics::Platform;

bool VulkanGraphicsManager::InitSubsystem(ECHAR* appTitle,
	int appVersionMajor, int appVersionMinor, int appVersionPatch,
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

	//Init Vulkan API	
	if (!InitVKInstance(appTitle, appVersionMajor, appVersionMinor, appVersionPatch))
		return false;
	if (!InitVKDevice())
		return false;

	//Done
	return true;
}

bool VulkanGraphicsManager::ShutdownSubsystem()
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager::ShutdownSubsystem()\n");	
	
	//Cleanup vulkan
	ShutdownVKDevice();
	ShutdownVKInstance();
	
	return true;
}

//
//VK Init
//

bool VulkanGraphicsManager::InitVKInstance(ECHAR* applicationTitle,
	int appVersionMajor, int appVersionMinor, int appVersionPatch)
{
	//
	//Decribes application & what vulkan API to use (minimum supported)
	//
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.apiVersion = VK_MAKE_VERSION(ENGINE_CONFIG_MINIMUM_VULKAN_API_MAJOR, ENGINE_CONFIG_MINIMUM_VULKAN_API_MINOR, ENGINE_CONFIG_MINIMUM_VULKAN_API_PATCH);
	appInfo.engineVersion = VK_MAKE_VERSION(ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, ENGINE_VERSION_PATCH);
	appInfo.pEngineName = ENGINE_NAME;
	appInfo.applicationVersion = VK_MAKE_VERSION(appVersionMajor, appVersionMinor, appVersionPatch);
	appInfo.pApplicationName = applicationTitle;

	//
	//How do we want to construct the vulkan instance
	//
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.pNext = nullptr;
	createInfo.flags = 0; //Not used as of yet in vulkan
	//***These maybe overriden!****
	createInfo.ppEnabledLayerNames = nullptr;
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledExtensionNames = nullptr;
	createInfo.enabledExtensionCount = 0;

	//********************************************************************************
	//********************************************************************************
	//************************Enabled layers (instance)*******************************
	//********************************************************************************
	//********************************************************************************
#if ENGINE_CONFIG_VULKAN_API_ENABLE_VALIDATION_AND_DEBUG_LAYERS_ON_INSTANCE
	std::vector<const char *> enabledInstanceLayers = 
	{
		"VK_LAYER_LUNARG_standard_validation", 
	};
#else
	std::vector<const char *> enabledInstanceLayers(0);
#endif

	//Validate
	if (!ValidateVKInstanceLayers(&enabledInstanceLayers))
		return false;

	if (enabledInstanceLayers.size() > 0)
	{
		createInfo.ppEnabledLayerNames = enabledInstanceLayers.data();
		createInfo.enabledLayerCount = enabledInstanceLayers.size();
	}

	//********************************************************************************
	//********************************************************************************
	//************************Enabled extentions (instance)***************************
	//********************************************************************************
	//********************************************************************************
	std::vector<const char *> enabledInstanceExtensions = 
	{
		VK_KHR_SURFACE_EXTENSION_NAME,

		//Win32 extention
#ifdef VK_USE_PLATFORM_WIN32_KHR 
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
		//Debug extentions
#if ENGINE_CONFIG_VULKAN_API_ENABLE_VALIDATION_AND_DEBUG_EXTENTIONS_ON_INSTANCE
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif
	};

	//Validate
	if (!ValidateVKInstanceExtentions(&enabledInstanceExtensions))
		return false;

	if (enabledInstanceExtensions.size() > 0)
	{
		createInfo.ppEnabledExtensionNames = enabledInstanceExtensions.data();
		createInfo.enabledExtensionCount = enabledInstanceExtensions.size();
	}

	//
	//Go ahead and now create the instance
	//
	VkResult result = vkCreateInstance(&createInfo, nullptr, &vkInstance);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsManager::InitVKInstance(): vkCreateInstance() failed.\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanGraphicsManager::InitVKDevice()
{
	//********************************************************************************
	//********************************************************************************
	//***********************Pick a physical device***********************************
	//********************************************************************************
	//********************************************************************************
	//
	//Get number of physical devices
	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, nullptr);

	//Ensure that we have at least one valid devices
	if (physicalDeviceCount == 0)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsManager::InitVKDevice(): 0 VK Physical Devices found in this system.\n");
		return false;
	} 
	else if (physicalDeviceCount > 1) //Just incase...
		EngineAPI::Debug::DebugLog::PrintWarningMessage("VulkanGraphicsManager::InitVKDevice(): > 1 VK Physical Devices found in this system!\n");

	//Get all the possible/available physical devices
	VkPhysicalDevice* vkPhysicalDevicesArray = GE_NEW VkPhysicalDevice[physicalDeviceCount];
	vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, &vkPhysicalDevicesArray[0]);

	//Pick the best...
	vkPhysicalDevice = PickBestVulkanPhysicalDevice(&vkPhysicalDevicesArray, physicalDeviceCount);

	//Cleanup memory holding all possible physical devices (we have cahced the handle of
	//our chosen device/GPU so this is fine)
	if (vkPhysicalDevicesArray)
		delete[] vkPhysicalDevicesArray;

	//********************************************************************************
	//********************************************************************************
	//*******************Enabled device layers (Depreciated??)************************
	//********************************************************************************
	//********************************************************************************
	//
	//Find all device layers available to us + number of them
	std::vector<const char*> deviceLayersNames =
	{};

	//Validate
	if (!ValidateVKDeviceLayers(&deviceLayersNames))
		return false;

	//********************************************************************************
	//********************************************************************************
	//**********************Enabled device extentions*********************************
	//********************************************************************************
	//********************************************************************************
	//
	//NOTE: Doesnt look for device extentions which are
	//provided by a layer -> Just finds the device extentions provided by the Vulkan
	//implementation
	//Pick some device extentions
	std::vector<const char *> deviceExtensionNames = 
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME, 
	};

	//Validate
	if (!ValidateVKDeviceExtentions(&deviceExtensionNames))
		return false;

	//********************************************************************************
	//********************************************************************************
	//****************Enabled features (API stuff we want access too)*****************
	//********************************************************************************
	//********************************************************************************
	//
	//Requested features
	VkPhysicalDeviceFeatures vkDeviceEnabledFeaturesArray = {};

	//Validate (TODO)
	//if (!ValidateVKDeviceFeatures(&vkDeviceEnabledFeaturesArray))
	//	return false;

	//********************************************************************************
	//********************************************************************************
	//**********************************Queue(s)**************************************
	//********************************************************************************
	//********************************************************************************
	//
	//Get the chosen devices queue families (count)
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr); //Count
	
	//Get all queue families
	VkQueueFamilyProperties* queueFamiliesArray = GE_NEW VkQueueFamilyProperties[queueFamilyCount];
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, &queueFamiliesArray[0]);

	//Search queue families for the graphics queue: VK_QUEUE_GRAPHICS_BIT
	uint32_t graphicsQueueHandle = 0;
	bool hasFoundQueue = false;
	hasFoundQueue = GetGraphicsQueueFamilyHandle(&queueFamiliesArray[0], queueFamilyCount, graphicsQueueHandle);

	//Check
	if (!hasFoundQueue)
	{
		//Fail!
	}

	//Graphics queue
	VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {};
	float queuePriorities[1] = { 0.0 };
	graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	graphicsQueueCreateInfo.pNext = nullptr;
	graphicsQueueCreateInfo.flags = 0; //Reserved
	graphicsQueueCreateInfo.queueFamilyIndex = graphicsQueueHandle;
	graphicsQueueCreateInfo.queueCount = ENGINE_CONFIG_VULKAN_API_GRAPHICS_QUEUE_COUNT;
	graphicsQueueCreateInfo.pQueuePriorities = queuePriorities;

	//TODO: Other queue families

	//Number of family queues we wish to create queues in
	uint32_t numFamilyQueuesToCreate = 0;
#if ENGINE_CONFIG_VULKAN_API_GRAPHICS_QUEUE_COUNT
	numFamilyQueuesToCreate++;
#endif 
#if ENGINE_CONFIG_VULKAN_API_COMPUTE_QUEUE_COUNT
	numFamilyQueuesToCreate++;
#endif 
#if ENGINE_CONFIG_VULKAN_API_TRANSFER_QUEUE_COUNT
	numFamilyQueuesToCreate++;
#endif 
#if ENGINE_CONFIG_VULKAN_API_SPARSE_QUEUE_COUNT
	numFamilyQueuesToCreate++;
#endif 

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
	vkDeviceCreateInfo.enabledLayerCount   = (uint32_t)deviceLayersNames.size();								   //Depreciated
	vkDeviceCreateInfo.ppEnabledLayerNames = (deviceLayersNames.size() > 0) ? deviceLayersNames.data() : NULL;     //Depreciated
	vkDeviceCreateInfo.enabledExtensionCount   = (uint32_t)deviceExtensionNames.size();									
	vkDeviceCreateInfo.ppEnabledExtensionNames = (deviceExtensionNames.size() > 0) ? deviceExtensionNames.data() : NULL;
	vkDeviceCreateInfo.pEnabledFeatures = &vkDeviceEnabledFeaturesArray;
	vkDeviceCreateInfo.pQueueCreateInfos    = &graphicsQueueCreateInfo;
	vkDeviceCreateInfo.queueCreateInfoCount = numFamilyQueuesToCreate;

	//Create logical device
	VkResult result = vkCreateDevice(vkPhysicalDevice, &vkDeviceCreateInfo, nullptr, &vkLogicalDevice);

	//Cleanup
	if (queueFamiliesArray)
		delete[] queueFamiliesArray;

	//Validate that the logical device was created
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsManager::InitVKDevice(): vkCreateDevice() failed.\n");
		return false;
	}

	//Done
	return true;
}

//
//VK Validation
//

bool VulkanGraphicsManager::ValidateVKInstanceLayers(std::vector<const char*> *desiredInstanceLayers)
{
	//No need to check...
	if (desiredInstanceLayers->size() == 0)
	{
#if SHOULD_PRINT_GRAPHICS_INIT_INFO
		EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager: Vulkan enabling NO instance layers\n");
#endif
		return true;
	}
	
	//Find all available instance layers
	uint32_t availInstanceLayerCount = 0;
	vkEnumerateInstanceLayerProperties(&availInstanceLayerCount, nullptr); //Count

	//Get them all 
	VkLayerProperties* availLayersArray = GE_NEW VkLayerProperties[availInstanceLayerCount];
	vkEnumerateInstanceLayerProperties(&availInstanceLayerCount, &availLayersArray[0]);

	//How many layers we are requesting have we validated exist in this driver (avilable for
	//us to enable)
	int layerAvailableMatchedWithRequestedCount = 0;

	//For each of the available instance layers...
	for (int i = 0; i < (int)availInstanceLayerCount; i++)
	{
		//ith layer...
		VkLayerProperties* layerProperty = &availLayersArray[i];

		//Does this layer name match with one of our requested layers. If so, increment a counter
		for (int j = 0; j < (int)desiredInstanceLayers->size(); j++)
		{
			if (strcmp(layerProperty->layerName, (*desiredInstanceLayers)[j]) == 0)
			{
#if SHOULD_PRINT_GRAPHICS_INIT_INFO
				EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager: Vulkan enabling instance layer: ");
				EngineAPI::Debug::DebugLog::PrintMessage(layerProperty->layerName);
				EngineAPI::Debug::DebugLog::PrintMessage("\n");
#endif
				layerAvailableMatchedWithRequestedCount++;
				break;
			}//if strcmp()
		}//for(j)
	}//for(i)

	//Cleanup memory
	delete[] availLayersArray;

	//Are all requested instance layers are available. 
	if (layerAvailableMatchedWithRequestedCount == desiredInstanceLayers->size())
		return true;
	else
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsManager: Error when validating Vulkan Instance Layers! ");
		EngineAPI::Debug::DebugLog::PrintMessage("It's likely that at least one requested instance layer is not avilable for you to use in this Vulkan implentation.\n");
		return false;
	}
}

bool VulkanGraphicsManager::ValidateVKInstanceExtentions(std::vector<const char*> *desiredInstanceExtentions)
{
	//No need to check...
	if (desiredInstanceExtentions->size() == 0)
	{
#if SHOULD_PRINT_GRAPHICS_INIT_INFO
		EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager: Vulkan enabling NO instance extentions\n");
#endif
		return true;
	}

	//Find all available instance extentions (provided by the Vulkan implementation. NOTE: the
	//first value (cstring) passed to vkEnumerateInstanceExtensionProperties could be the
	//name of a layer - this function would then return the instance extentions provided
	//as a result of that layer...).
	uint32_t availInstanceExtentionsCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &availInstanceExtentionsCount, nullptr); //Count

	//Get all instance extentions
	VkExtensionProperties* availInstanceExtentions = GE_NEW VkExtensionProperties[availInstanceExtentionsCount];
	vkEnumerateInstanceExtensionProperties(nullptr, &availInstanceExtentionsCount, &availInstanceExtentions[0]);

	//Are the instance extentions we want available for us to use...
	int instanceExtentionsAvailToRequestedCounter = 0;
	for (int i = 0; i < (int)availInstanceExtentionsCount; i++)
	{
		//ith instance extention...
		VkExtensionProperties* instanceExtention = &availInstanceExtentions[i];
		
		//Does this match (by name) with a requested extention. If so, increment a counter.
		for (int j = 0; j < (int)desiredInstanceExtentions->size(); j++)
		{
			if (strcmp(instanceExtention->extensionName, (*desiredInstanceExtentions)[j]) == 0)
			{
#if SHOULD_PRINT_GRAPHICS_INIT_INFO
				EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager: Vulkan enabling instance extention: ");
				EngineAPI::Debug::DebugLog::PrintMessage(instanceExtention->extensionName);
				EngineAPI::Debug::DebugLog::PrintMessage("\n");
#endif
				//Match
				instanceExtentionsAvailToRequestedCounter++;
				break;
			}//Strcmp()
		}//for(j)
	}//for(i)

	//Cleanup memory
	delete[] availInstanceExtentions;

	//Do validation
	if (instanceExtentionsAvailToRequestedCounter == desiredInstanceExtentions->size())
		return true;
	else
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsManager: Error when validating Vulkan Instance Extentions! ");
		EngineAPI::Debug::DebugLog::PrintMessage("It's likely that at least one requested instance extention is not avilable for you to use in this Vulkan implentation.\n");
		return false;
	}
}

bool VulkanGraphicsManager::ValidateVKDeviceLayers(std::vector<const char*> *desiredDeviceLayers)
{
	//No need to check...
	if (desiredDeviceLayers->size() == 0)
	{
#if SHOULD_PRINT_GRAPHICS_INIT_INFO
		EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager: Vulkan enabling NO device layers\n");
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
				EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager: Vulkan enabling device layer: ");
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
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsManager: Error when validating Vulkan Device Layers! ");
		EngineAPI::Debug::DebugLog::PrintMessage("It's likely that at least one requested device layer is not avilable for you to use in this Vulkan implentation.\n");
		return false;
	}
}

bool VulkanGraphicsManager::ValidateVKDeviceExtentions(std::vector<const char*> *desiredDeviceExtentions)
{
	//No need to check...
	if (desiredDeviceExtentions->size() == 0)
	{
#if SHOULD_PRINT_GRAPHICS_INIT_INFO
		EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager: Vulkan enabling NO device extentions\n");
#endif
		return true;
	}

	//Get the device extentions count
	uint32_t deviceExtentionsCount = 0;
	vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, NULL, &deviceExtentionsCount, nullptr);
	VkExtensionProperties* availDeviceExtentions = nullptr;
	
	//Get all device extentions
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
				EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager: Vulkan enabling device extention: ");
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
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsManager: Error when validating Vulkan Device Extentions! ");
		EngineAPI::Debug::DebugLog::PrintMessage("It's likely that at least one requested device extention is not avilable for you to use in this Vulkan implentation.\n");
		return false;
	}
}

/*
bool VulkanGraphicsManager::ValidateVKDeviceFeatures(VkPhysicalDeviceFeatures* requestedDeviceFeatures)
{
	//TODO
	return true;
}
*/

//
//VK Helper
//

VkPhysicalDevice VulkanGraphicsManager::PickBestVulkanPhysicalDevice(VkPhysicalDevice** availPhysicalDevices,
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

#if SHOULD_PRINT_GRAPHICS_INIT_INFO 
	//Info to print
	VkPhysicalDeviceProperties deviceProperties{};
	vkGetPhysicalDeviceProperties(vkPickedPhysicalDevice, &deviceProperties);

	//String
	char str[256];
	int major = 0;
	int minor = 0;
	int patch = 0;

	//Device name
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager: Vulkan Device Selected: ");
	EngineAPI::Debug::DebugLog::PrintMessage(deviceProperties.deviceName);
	EngineAPI::Debug::DebugLog::PrintMessage("\n");

	//Device type
	sprintf(&str[0], "%i", deviceProperties.deviceType);
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager: Vulkan Device Type (VkPhysicalDeviceType Enum): ");
	EngineAPI::Debug::DebugLog::PrintMessage(str);
	EngineAPI::Debug::DebugLog::PrintMessage("\n");

	//Limits - device bytes
	uint64_t deviceBytes = deviceProperties.limits.sparseAddressSpaceSize;
	sprintf(&str[0], "%u", deviceBytes);
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager: Maximum Available Memory: ");
	EngineAPI::Debug::DebugLog::PrintMessage(str);
	EngineAPI::Debug::DebugLog::PrintMessage("\n");

	//Driver version
	sprintf(&str[0], "%i.%i", VK_VERSION_MAJOR(deviceProperties.driverVersion), 
		VK_VERSION_MINOR(deviceProperties.driverVersion));
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager: Vulkan Device Driver Version: ");
	EngineAPI::Debug::DebugLog::PrintMessage(str);
	EngineAPI::Debug::DebugLog::PrintMessage("\n");

	//Device ID && Vendor ID
	sprintf(&str[0], "%i. VendorID: %i", deviceProperties.deviceID, deviceProperties.vendorID);
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager: Vulkan Device ID: ");
	EngineAPI::Debug::DebugLog::PrintMessage(str);
	EngineAPI::Debug::DebugLog::PrintMessage("\n");
	
	//API version
	sprintf(&str[0], "%i.%i.%i", VK_VERSION_MAJOR(deviceProperties.apiVersion),
		VK_VERSION_MINOR(deviceProperties.apiVersion), 
		VK_VERSION_PATCH(deviceProperties.apiVersion));
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager: Vulkan Device API Version: ");
	EngineAPI::Debug::DebugLog::PrintMessage(str);
	EngineAPI::Debug::DebugLog::PrintMessage("\n");

#endif

	//Done
	return vkPickedPhysicalDevice;
}

bool VulkanGraphicsManager::GetGraphicsQueueFamilyHandle(
	VkQueueFamilyProperties* deviceQueueFamiliesArray, uint32_t queueFamilyCount,
	uint32_t& graphicsHandleOut)
{
	for (int i = 0; i < queueFamilyCount; i++)
	{
		if (deviceQueueFamiliesArray[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			//Get handle and return true
			graphicsHandleOut = i;
			return true;
		}
	}

	//False
	graphicsHandleOut = 0;
	return false;
}

//
//VK Shutdown
//

void VulkanGraphicsManager::ShutdownVKInstance()
{
	vkDestroyInstance(vkInstance, nullptr);
	vkInstance = NULL;
}

void VulkanGraphicsManager::ShutdownVKDevice()
{
	vkDestroyDevice(vkLogicalDevice, nullptr);
	vkLogicalDevice = NULL;
	vkPhysicalDevice = NULL;
}