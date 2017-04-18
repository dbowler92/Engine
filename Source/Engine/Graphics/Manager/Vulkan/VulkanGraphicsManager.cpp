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
	VkApplicationInfo appInfo{};
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
	VkInstanceCreateInfo createInfo{};
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
		"VK_LAYER_LUNARG_standard_validation"
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
	VkResult result;

	//
	//Pick physical device
	//
	VkPhysicalDevice vkPickedPhysicalDevice = nullptr;
	uint32_t physicalDeviceCount = 0;
	
	//Get number of physical devices
	vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, nullptr);

	//Get the devices
	std::vector<VkPhysicalDevice*>vkPhysicalDevicesArray(physicalDeviceCount);
	vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, *vkPhysicalDevicesArray.data());

	//
	//Device extentions
	//

	//
	//Device layers
	//

	//
	//Enabled features
	//

	//
	//Queues
	//

	//Description
	VkDeviceCreateInfo vkDeviceCreateInfo{};
	vkDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	vkDeviceCreateInfo.pNext = nullptr;
	vkDeviceCreateInfo.flags = 0; //Not used in VK 1.0.0
	//TODO

	//Create logical device
	result = vkCreateDevice(vkPickedPhysicalDevice, &vkDeviceCreateInfo, nullptr, &vkDevice);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsManager::InitVKDevice(): vkCreateDevice() failed.\n");
		return false;
	}
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
	for (int i = 0; i < availInstanceLayerCount; i++)
	{
		//ith layer...
		VkLayerProperties* layerProperty = &availLayersArray[i];

		//Does this layer name match with one of our requested layers. If so, increment a counter
		for (int j = 0; j < desiredInstanceLayers->size(); j++)
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
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsManager: Error when validating Vulkan Instance Layers!\n");
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
	for (int i = 0; i < availInstanceExtentionsCount; i++)
	{
		//ith instance extention...
		VkExtensionProperties* instanceExtention = &availInstanceExtentions[i];
		
		//Does this match (by name) with a requested extention. If so, increment a counter.
		for (int j = 0; j < desiredInstanceExtentions->size(); j++)
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
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsManager: Error when validating Vulkan Instance Extentions!\n");
		return false;
	}
}

//
//VK Shutdown
//

void VulkanGraphicsManager::ShutdownVKInstance()
{
	vkDestroyInstance(vkInstance, nullptr);
	vkInstance = nullptr;
}

void VulkanGraphicsManager::ShutdownVKDevice()
{
	vkDestroyDevice(vkDevice, nullptr);
	vkDevice = nullptr;
}