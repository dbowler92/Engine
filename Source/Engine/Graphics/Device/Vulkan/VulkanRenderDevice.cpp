#include "VulkanRenderDevice.h"

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

bool VulkanRenderDevice::Init(EngineAPI::OS::OSWindow* osWindow, 
	EngineAPI::Graphics::RenderInstance* renderingInstance)
{
	//********************************************************************************
	//********************************************************************************
	//***********************Pick a physical device***********************************
	//********************************************************************************
	//********************************************************************************
	//
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

	//********************************************************************************
	//********************************************************************************
	//*******************Enabled device layers (Depreciated??)************************
	//********************************************************************************
	//********************************************************************************
	//
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
	//
	//Validate
	if (!ValidateVKDeviceExtentions(renderingInstance->GetVKEnabledInstanceLayersList(), &deviceExtensionNames))
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
	//******************************Graphics Queue************************************
	//********************************************************************************
	//********************************************************************************

	//Search queue families for the graphics queue: VK_QUEUE_GRAPHICS_BIT
	bool hasFoundGraphicsQueue = false;
	uint32_t graphicsQueueFamilyIdx = 0;
	hasFoundGraphicsQueue = GetGraphicsQueueFamilyHandle(&vkQueueFamiliesArray[0], vkQueueFamiliesCount, &graphicsQueueFamilyIdx);

	//Check
	if (!hasFoundGraphicsQueue)
		return false;

	//Graphics queue creation - returns the struct needed when creating the 
	//logical device. Note: Untill we need a queue with support
	//for comoute, we don't need to change VkDeviceQueueCreateInfo::pNext!
	VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {};
	float queuePriorities[1] = { 0.0 };
	vkGraphicsQueueFamily.InitVulkanQueueFamily(QUEUE_FAMILY_SUPPORT_GRAPHICS,
		graphicsQueueFamilyIdx,
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
	vkDeviceCreateInfo.pQueueCreateInfos = &graphicsQueueCreateInfo;     //Just graphics
	vkDeviceCreateInfo.queueCreateInfoCount = 1;					     //Just graphics

	//Create logical device
	VkResult result = vkCreateDevice(vkPhysicalDevice, &vkDeviceCreateInfo, nullptr, &vkLogicalDevice);

	//Validate that the logical device was created
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderDevice::Init(): vkCreateDevice() failed.\n");
		return false;
	}

	//********************************************************************************
	//********************************************************************************
	//*********************************Memory*****************************************
	//********************************************************************************
	//********************************************************************************
	//
	//Cache memory info for the physical device
	CacheVKDeviceMemoryInfo();
	
	//TODO: Decide on best heap / types (by index) to use when allocating data
	//in specific ways for this device (eg: Differnt memoryType index will be used
	//during vkAlloc...() depending on if the memory should be mappable or not)
	if (!FindVKMemoryTypeIndexForEfficientDeviceOnlyAllocations())
		return false;
	if (!FindVKMemoryTypeIndexForMappableAllocations())
		return false;


	//********************************************************************************
	//********************************************************************************
	//***************************Command buffer pools*********************************
	//********************************************************************************
	//********************************************************************************
	//
	//Init vulkan command buffer pools
	if (!InitVKCommandBufferPools())
		return false;

	//********************************************************************************
	//********************************************************************************
	//********************************Final init**************************************
	//********************************************************************************
	//********************************************************************************
	//
	//Cache queue handle object(s)
	if (!vkGraphicsQueueFamily.InitVulkanQueues(&vkLogicalDevice))
		return false;

	//
	//
	//TESTING
	//
	//
	//RenderCommandBuffer* cmdBuffer = vkCommandBufferPoolsArray[0].AllocCommandBuffer(true);
	//cmdBuffer->BeginRecordingToCommandBuffer();
	//cmdBuffer->EndRecordingToCommandBuffer();

	RenderCommandBuffer* cmdBufferSingle = vkCommandBufferPoolsArray[0].AllocCommandBuffer(true);
	RenderCommandBuffer *cmdBuffersArray;
	bool primaryFlags[2] = { true, false };
	cmdBuffersArray = vkCommandBufferPoolsArray[0].AllocCommandBuffersArray(2, primaryFlags);

	if (!cmdBuffersArray[0].ResetCommandBuffer())
	{
		int x = 3123123;
		x++;
	}
	if (!vkCommandBufferPoolsArray[0].ResetCommandBufferPool())
	{
		int x = 90;
		x++; 
	}

	GE_DELETE cmdBufferSingle;
	GE_DELETE_ARRAY cmdBuffersArray;

	//Done
	return true;
}

void VulkanRenderDevice::Shutdown()
{
	//Cleanup command buffer pools
	if (vkCommandBufferPoolsArray)
	{
		for (int i = 0; i < ENGINE_CONFIG_VULKAN_API_GRAPHICS_COMMAND_BUFFER_POOLS_COUNT; i++)
			vkCommandBufferPoolsArray[i].Shutdown();
		delete[] vkCommandBufferPoolsArray;
	}

	//Queues
	vkGraphicsQueueFamily.Shutdown();
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

bool VulkanRenderDevice::FindVKMemoryTypeIndexForEfficientDeviceOnlyAllocations()
{
	//Look for the memory types that have the flag VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	for (int i = 0; i < vkDeviceMemoryProperties.memoryTypeCount; i++)
	{
		if (vkDeviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
		{
			//This types heap index
			uint32_t heapIndex = vkDeviceMemoryProperties.memoryTypes[i].heapIndex;

			//Is this heap device local (Should always be - but just incase)
			if (vkDeviceMemoryProperties.memoryHeaps[heapIndex].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
			{
				//Yes - this is the first (and thus best) memory 'type' to be using
				vkMemoryTypeIndexForEfficientDeviceOnlyAllocations = i;
				return true;
			}
		}
	}

	//Failed
	EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderDevice: Error. Couldn't find a memory type to be used for GPU only allocations.\n");
	return false;
}

bool VulkanRenderDevice::FindVKMemoryTypeIndexForMappableAllocations()
{
	//TODO
	return true;

	//Failed
	EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderDevice: Error. Couldn't find a memory type to be used for mappable only allocations.\n");
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

bool VulkanRenderDevice::GetGraphicsQueueFamilyHandle(VkQueueFamilyProperties* deviceQueueFamiliesArray, uint32_t queueFamilyCount, 
	uint32_t* graphicsQueueFamilyIndexOut)
{
	for (int i = 0; i < queueFamilyCount; i++)
	{
		if (deviceQueueFamiliesArray[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			//Store handle and return true
			*graphicsQueueFamilyIndexOut = i;
			return true;
		}
	}

	//False
	EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderDevice::GetGraphicsQueueFamilyHandle(): Failed to find the graphics queue family.\n");
	*graphicsQueueFamilyIndexOut = 0;
	return false;
}

bool VulkanRenderDevice::InitVKCommandBufferPools()
{
	//*************************************************************************************
	//*************************************************************************************
	//**************************GRAPHICS QUEUE FAMILY**************************************
	//*************************************************************************************
	//*************************************************************************************

	//Alloc
	vkCommandBufferPoolsArray = GE_NEW CommandBufferPool[ENGINE_CONFIG_VULKAN_API_GRAPHICS_COMMAND_BUFFER_POOLS_COUNT];
	if (!vkCommandBufferPoolsArray)
	{
		//Alloc error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderDevice: Error allocating memory for (VK) Command Buffer Pools\n");
		return false;
	}

	//Command pool alloc info.
	uint32_t cmdPoolSubmissionQueueFamilyIdxGraphics = vkGraphicsQueueFamily.GetVKQueueFamilyIndex();

	for (int i = 0; i < ENGINE_CONFIG_VULKAN_API_GRAPHICS_COMMAND_BUFFER_POOLS_COUNT; i++)
	{
		if (!vkCommandBufferPoolsArray[i].InitVKCommandBufferPool(&vkLogicalDevice, cmdPoolSubmissionQueueFamilyIdxGraphics,
			ENGINE_CONFIG_VULKAN_API_GRAPHICS_COMMAND_BUFFER_POOLS_ALLOW_BUFFER_RESETS,
			ENGINE_CONFIG_VULKAN_API_GRAPHICS_COMMAND_BUFFER_POOLS_IS_TRANSIENT))
		{
			//Init error
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanRenderDevice: Error initing (VK) Command Buffer Pools\n");
			return false;
		}
	}

	//Done
	return true;
}