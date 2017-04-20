#include "VulkanRenderInstance.h"

using namespace EngineAPI::Graphics::Platform;

bool VulkanRenderInstance::Init(EngineAPI::OS::OSWindow* osWindow, ECHAR* applicationTitle,
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
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderInstance::Init(): vkCreateInstance() failed.\n");
		return false;
	}

	return true;
}

void VulkanRenderInstance::Shutdown()
{
	vkDestroyInstance(vkInstance, nullptr);
	vkInstance = NULL;
}

bool VulkanRenderInstance::ValidateVKInstanceLayers(std::vector<const char*> *desiredInstanceLayers)
{
	//No need to check...
	if (desiredInstanceLayers->size() == 0)
	{
#if SHOULD_PRINT_GRAPHICS_INIT_INFO
		EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanRenderInstance: Vulkan enabling NO instance layers\n");
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
				EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanRenderInstance: Vulkan enabling instance layer: ");
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
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderInstance: Error when validating Vulkan Instance Layers! ");
		EngineAPI::Debug::DebugLog::PrintMessage("It's likely that at least one requested instance layer is not avilable for you to use in this Vulkan implentation.\n");
		return false;
	}
}

bool VulkanRenderInstance::ValidateVKInstanceExtentions(std::vector<const char*> *desiredInstanceExtentions)
{
	//No need to check...
	if (desiredInstanceExtentions->size() == 0)
	{
#if SHOULD_PRINT_GRAPHICS_INIT_INFO
		EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanRenderInstance: Vulkan enabling NO instance extentions\n");
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
				EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanRenderInstance: Vulkan enabling instance extention: ");
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
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderInstance: Error when validating Vulkan Instance Extentions! ");
		EngineAPI::Debug::DebugLog::PrintMessage("It's likely that at least one requested instance extention is not avilable for you to use in this Vulkan implentation.\n");
		return false;
	}
}