//VulkanRenderDevice.h
//Created 20/04/17
//Created By Daniel Bowler
//
//Represents both the Vulkan physical device (the GPU or whatever) and the 
//Vulkan logical device (needed for rendering, loading etc)

#pragma once

//Needs to know about the OS window
#include "../../../OSWindow/OSWindow.h"

//May need the rendering instance to create the device
#include "../../Instance/RenderInstance.h"

//Manages queue family and allocator
#include "../../CommandQueueFamily/CommandQueueFamily.h"
#include "../../DeviceMemoryAllocator/DeviceMemoryAllocator.h"

//Vulkan header
#include <vulkan\vulkan.h>

//Forward declarations
namespace EngineAPI
{
	namespace Graphics
	{
		class CommandQueueFamily;
		class CommandBufferPool;
		class DeviceMemoryAllocator;
	};
};

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanRenderDevice
			{
			public:
				VulkanRenderDevice() {};
				virtual ~VulkanRenderDevice() = 0 {};

				//Shutdown the device
				void Shutdown();

				//Inits vulkan step by step
				bool InitVKPhysicalDevice(EngineAPI::OS::OSWindow* osWindow,
					EngineAPI::Graphics::RenderInstance* renderingInstance);
				bool InitVKLogicalDeviceAndQueues(EngineAPI::OS::OSWindow* osWindow,
					EngineAPI::Graphics::RenderInstance* renderingInstance, const VkSurfaceKHR logicalSurfaceHandle);
				bool InitVKMemoryAllocator(EngineAPI::OS::OSWindow* osWindow,
					EngineAPI::Graphics::RenderInstance* renderingInstance);
				bool InitVKCommandBufferPools(EngineAPI::OS::OSWindow* osWindow,
					EngineAPI::Graphics::RenderInstance* renderingInstance);

				//Called when resize the window. Currently, this just returns true. However, in the
				//future, we may wish to delete some memory stores and resize them as a result of this. 
				bool OnResize(ESize2D newSize);

			public:
				//Vulkan getters:
				//
				//Physical and logical device handles
				VkPhysicalDevice GetVKPhysicalDevice() { return vkPhysicalDevice; };
				VkDevice GetVKLogicalDevice() { return vkLogicalDevice; };

				//Stored info on physical and logical device
				VkPhysicalDeviceProperties GetVKPhysicalDeviceProperties() { return vkDeviceProperties; };
				VkPhysicalDeviceMemoryProperties GetVKPhysicalDeviceMemoryProperties() { return vkDeviceMemoryProperties; };

			public:
				//Returns the managers
				//
				//Allocator 
				EngineAPI::Graphics::DeviceMemoryAllocator* GetDeviceMemoryAllocator() { return &deviceMemoryAllocator; };

				//Command queue families
				EngineAPI::Graphics::CommandQueueFamily* GetGraphicsCommandQueueFamily() { return &graphicsQueueFamily; };
				EngineAPI::Graphics::CommandQueueFamily* GetPresentCommandQueueFamily() { return &graphicsQueueFamily; };  //Currently queue handles graphics *AND* present work
				EngineAPI::Graphics::CommandQueueFamily* GetDMATransferCommandQueueFamily() { return &graphicsQueueFamily; }; //Graphics queue can handle transfers. In future, this could be a separate queue family!

			private:
				//Vulkan data / handles
				VkPhysicalDevice vkPhysicalDevice = NULL;
				VkDevice vkLogicalDevice = NULL;

				//Device properties
				VkPhysicalDeviceProperties vkDeviceProperties;			   //General info
				VkPhysicalDeviceMemoryProperties vkDeviceMemoryProperties; //Device memory info
				
				VkPhysicalDeviceFeatures vkDeviceAvailableFeatures;        //All available features for this physical device
				VkPhysicalDeviceFeatures vkDeviceEnabledFeaturesArray;     //Manually enabled device features. 

				 //*ALL* Device families queues supported by this physical device
				VkQueueFamilyProperties* vkQueueFamiliesArray = nullptr;
				uint32_t vkQueueFamiliesCount; //Number of queue families exposed by our selected physical device

			private:
				//Device memory manager / allocator
				EngineAPI::Graphics::DeviceMemoryAllocator deviceMemoryAllocator;

				//Graphics queue family and queues - general rendering
				EngineAPI::Graphics::CommandQueueFamily graphicsQueueFamily;
								
			private:
				//Function pointers to extensions (WSI in this case - helps us create swapchains etc).
				//
				//Can ask a queue family if it supports presentation work!
				PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR = VK_NULL_HANDLE;

			private:
				//Vk allocation/init
				//
				//Caches info regarding the Vulkan device memory capabilities. 
				void CacheVKDeviceMemoryInfo();

				//Finds a memoryType index which supports a set of properties
				bool FindVKMemoryTypeIndexForProperties(VkMemoryPropertyFlags properties, uint32_t* memoryTypeIndexOut);

				//Finds the "best" Vulkan enabled physical device for us to use
				//when creating the Vulkan physical device. Currently, it just picks the
				//first discrete GPU which should be fine for us.
				//
				//See comments for a better approach. 
				VkPhysicalDevice PickBestVulkanPhysicalDevice(VkPhysicalDevice** availPhysicalDevices,
					uint32_t availPhysicalDevicesCount);

				//Gets the queue handle for a given type. 
				//
				//Finds queue for graphics and queue for presentation (Ideally the same!)
				bool GetGraphicsAndPresentQueueFamilyHandle(const VkSurfaceKHR logicalSurface,
					VkQueueFamilyProperties* deviceQueueFamiliesArray, uint32_t queueFamilyCount, 
					uint32_t* graphicsQueueFamilyIndexOut, uint32_t* presentQueeuFamilyIndexOut);

			private:
				//Validates our chosen device extentions & layers (Depreciated I think. However, 
				//I will add the code in needed just incase!)- Are the ones
				//we want available to us? Again, doesnt attempt to recover in the
				//case they do not exist. 
				bool ValidateVKDeviceLayers(std::vector<const char*> *desiredDeviceLayers);
				bool ValidateVKDeviceExtentions(std::vector<const char*> *enabledInstanceLayers,
					std::vector<const char*> *desiredDeviceExtentions);

				//Again, like above, this function checks to see if our device can
				//support the requested API feature set. NOTE: This likely should
				//be done before/at the same time we pick a specific device -> Thus, we
				//could reject a potential device that doesn't support the requested features
				//of our game/app/engine. 
				//
				//IMPORTANT: As VK is new, this function is pointless (all devices will
				//support the stuff we need) and doesnt actually do anything 
				//(instantly return true).
				//bool ValidateVKDeviceFeatures(VkPhysicalDeviceFeatures* requestedDeviceFeatures);
			};
		};
	};
};