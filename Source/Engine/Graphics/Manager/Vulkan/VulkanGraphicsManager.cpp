#include "VulkanGraphicsManager.h"
#include <iostream>

using namespace EngineAPI::Graphics::Platform;

bool VulkanGraphicsManager::InitSubsystem(ECHAR* appTitle,
	int appVersionMajor, int appVersionMinor, int appVersionPatch,
	unsigned screenWidth, unsigned screenHeight)
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager::InitSubsystem()\n");

#if SHOULD_PRINT_GRAPHICS_INIT_INFO
	//Debug string
	char vkInfoMsg[256];
#endif

	//Init Vulkan API	
	VkResult result = InitVKInstance(appTitle, appVersionMajor, appVersionMinor, appVersionPatch);
	result = InitVKDevice();

	/*
	//1) Enumerate instance layer properties
	//
	//Number of instance layers available
	uint32_t instanceLayerCount = 0;
	VkLayerProperties* layerPropertyArr = nullptr;
	result = vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr); //Gets the count

#if SHOULD_PRINT_GRAPHICS_INIT_INFO
	sprintf(&vkInfoMsg[0], "VulkanGraphicsManager: VK Layer Property Count: %i\n", (int)instanceLayerCount);
	EngineAPI::Debug::DebugLog::PrintInfoMessage(vkInfoMsg);
#endif 

	//Get all instance layers
	layerPropertyArr = GE_NEW VkLayerProperties[instanceLayerCount];
	vkEnumerateInstanceLayerProperties(&instanceLayerCount, layerPropertyArr);

	//For each instance layer, get extentions 
	for (int i = 0; i < instanceLayerCount; i++)
	{
		
	}

	//Cleanup
	delete[] layerPropertyArr;
	*/

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

VkResult VulkanGraphicsManager::InitVKInstance(ECHAR* applicationTitle, 
	int appVersionMajor, int appVersionMinor, int appVersionPatch)
{
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
	//Decribes application & what vulkan API to use (minimum supported)
	VkApplicationInfo appInfo{};
	appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext              = nullptr;
	appInfo.apiVersion         = VK_MAKE_VERSION(ENGINE_CONFIG_MINIMUM_VULKAN_API_MAJOR,ENGINE_CONFIG_MINIMUM_VULKAN_API_MINOR,  ENGINE_CONFIG_MINIMUM_VULKAN_API_PATCH);
	appInfo.engineVersion      = VK_MAKE_VERSION(ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, ENGINE_VERSION_PATCH);
	appInfo.pEngineName        = ENGINE_NAME;
	appInfo.applicationVersion = VK_MAKE_VERSION(appVersionMajor, appVersionMinor, appVersionPatch);
	appInfo.pApplicationName   = applicationTitle;

#if SHOULD_PRINT_GRAPHICS_INIT_INFO
	char vkInfoMsg[256];

	//Engine version
	sprintf(&vkInfoMsg[0], "VulkanGraphicsManager: Game Engine Build: %s. Version: %i, %i, %i\n",
		ENGINE_NAME, ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, ENGINE_VERSION_PATCH);
	EngineAPI::Debug::DebugLog::PrintInfoMessage(vkInfoMsg);

	//App Version
	sprintf(&vkInfoMsg[0], "VulkanGraphicsManager: Application Build: %s. Version: %i, %i, %i\n",
		applicationTitle, appVersionMajor, appVersionMinor, appVersionPatch);
	EngineAPI::Debug::DebugLog::PrintInfoMessage(vkInfoMsg);

	//Minimum vulkan API required
	sprintf(&vkInfoMsg[0], "VulkanGraphicsManager: Minimum Vulkan API Supported: %i, %i, %i\n",
		ENGINE_CONFIG_MINIMUM_VULKAN_API_MAJOR, ENGINE_CONFIG_MINIMUM_VULKAN_API_MINOR, ENGINE_CONFIG_MINIMUM_VULKAN_API_PATCH);
	EngineAPI::Debug::DebugLog::PrintInfoMessage(vkInfoMsg);
#endif

	//Enabled layers
	//

	//Enabled extentions
	//

	VkInstanceCreateInfo createInfo {};
	createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.pNext            = nullptr;
	createInfo.flags            = 0; //Not used as of yet in vulkan

	//Create the instance
	VkResult result = vkCreateInstance(&createInfo, nullptr, &vkInstance);
	if (result != VK_SUCCESS)
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsManager::InitVKInstance(): vkCreateInstance() failed.\n");

	//Done
	return result;
#endif
}

VkResult VulkanGraphicsManager::InitVKDevice()
{
	VkResult result;

	return result;
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

}