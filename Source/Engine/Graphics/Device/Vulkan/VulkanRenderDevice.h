//VulkanRenderDevice.h
//Created 20/04/17
//Created By Daniel Bowler
//
//Represents both the Vulkan physical device (the GPU or whatever) and the 
//Vulkan logical device (needed for rendering, loading etc)

#pragma once

//Common interface
#include "../Common/CommonRenderDevice.h"

//Vulkan header
#include <vulkan\vulkan.h>

//Forward declarations
namespace EngineAPI
{
	namespace Graphics
	{
		class CommandQueueFamily;
		class CommandBufferPool;
		class DeviceMemoryBlock;
	};
};

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanRenderDevice : public EngineAPI::Graphics::Interface::CommonRenderDevice
			{
			public:
				VulkanRenderDevice() {};
				virtual ~VulkanRenderDevice() = 0 {};

				//Shutdown the device
				void Shutdown() override;

				//Inits vulkan step by step
				bool InitVKPhysicalDevice(EngineAPI::OS::OSWindow* osWindow,
					EngineAPI::Graphics::RenderInstance* renderingInstance);
				bool InitVKLogicalDeviceAndQueues(EngineAPI::OS::OSWindow* osWindow,
					EngineAPI::Graphics::RenderInstance* renderingInstance, const VkSurfaceKHR logicalSurfaceHandle);
				bool InitVKMemoryBlocks(EngineAPI::OS::OSWindow* osWindow,
					EngineAPI::Graphics::RenderInstance* renderingInstance);
				bool InitVKCommandBufferPools(EngineAPI::OS::OSWindow* osWindow,
					EngineAPI::Graphics::RenderInstance* renderingInstance);

			public:
				//Vulkan getters:
				//
				//Physical and logical device handles
				VkPhysicalDevice GetVKPhysicalDevice() { return vkPhysicalDevice; };
				VkDevice GetVKLogicalDevice() { return vkLogicalDevice; };

			private:
				//Vulkan data / handles
				VkPhysicalDevice vkPhysicalDevice = NULL;
				VkDevice vkLogicalDevice = NULL;

				//Device properties
				VkPhysicalDeviceProperties vkDeviceProperties;			   //General info
				VkPhysicalDeviceMemoryProperties vkDeviceMemoryProperties; //Device memory info

				 //*ALL* Device families queues
				VkQueueFamilyProperties* vkQueueFamiliesArray = nullptr;
				uint32_t vkQueueFamiliesCount; //Number of queue families exposed by our selected physical device

			private:
				//Function pointers to extensions (WSI in this case - helps us create swapchains etc).
				//
				//Can ask a queue family if it supports presentation work!
				PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR = VK_NULL_HANDLE;

			private:
				//Vulkan command buffer pool(s)
				CommandBufferPool* commandBufferPoolsArray = nullptr;

				//Graphics queue family and queues - general rendering
				CommandQueueFamily* graphicsQueueFamily = nullptr;

				//Device (GPU) Memory block(s) - used to alloc resources on the GPU. Different blocks
				//support different things. (eg: Mappable memory, etc)
				//
				//TEMP: Just alloc the full memory & use this to suballoc as and when required. Later, 
				//we will want to improve this by having separate blocks for different tasks. 
				DeviceMemoryBlock* deviceMemoryBlock = nullptr;

			private:
				//Vk allocation/init
				//
				//Caches info regarding the Vulkan device memory capabilities. 
				void CacheVKDeviceMemoryInfo();

				//From the cached memory info for the given device, these function
				//finds the best memory types to be used when allocating resources
				//
				//Returns false if no valid memory type && heap can be found
				bool FindVKMemoryTypeIndexForEfficientDeviceOnlyAllocations(uint32_t* memoryTypeIndexOut);
				bool FindVKMemoryTypeIndexForMappableAllocations(uint32_t* memoryTypeIndexOut);

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