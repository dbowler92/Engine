//VulkanRenderDevice.h
//Created 20/04/17
//Created By Daniel Bowler
//
//Represents both the Vulkan physical device (the GPU or whatever) and the 
//Vulkan logical device (needed for rendering, loading etc)

#pragma once

//Common interface
#include "../Common/CommonRenderDevice.h"

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

				//Inits the device & shutsdown the device
				bool Init(EngineAPI::OS::OSWindow* osWindow, 
					EngineAPI::Graphics::RenderInstance* renderingInstance) override;
				void Shutdown() override;

			public:
				//Important functions to interact with the Vulkan device. Eg: Command buffer 
				//creation/allocation


			public:
				//Vulkan getters:
				//
				//Physical and logical device handles
				VkPhysicalDevice GetVKPhysicalDevice() { return vkPhysicalDevice; };
				VkDevice GetVKLogicalDevice() { return vkLogicalDevice; };

				//Picked physical device properties such as memory, name etc 
				VkPhysicalDeviceProperties GetVKPhysicalDeviceProperties() { return vkDeviceProperties; }
				VkPhysicalDeviceMemoryProperties GetVKPhysicalDeviceMemoryProperties() { return vkDeviceMemoryProperties; };

				//Queue families exposed by the picked physical device
				VkQueueFamilyProperties* GetVKQueueFamiliesProperties() { return vkQueueFamiliesArray; };
				uint32_t GetVKQueueFamiliesCount() { return vkQueueFamiliesCount; }
				
				//Index in to our array of queue families of the queue family responsable
				//for general graphics work
				//
				//Second function returns the VkQueue object/handle (representing an individual a queue) 
				//of the graphics processing queue - pass an index since it is possible
				//for us to generate several queues for this given queue type/family
				uint32_t GetVKGraphicsQueueFamilyIndex() { return vkGraphicsQueueFamilyIndex; };
				VkQueue GetVKGraphicsQueueHandleByIndex(uint32_t idx = 0) { return vkGraphicsQueue[idx]; };

			private:
				//Vulkan data / handles
				VkPhysicalDevice vkPhysicalDevice = NULL;
				VkDevice vkLogicalDevice = NULL;

				//Device properties
				VkPhysicalDeviceProperties vkDeviceProperties;			   //General info
				VkPhysicalDeviceMemoryProperties vkDeviceMemoryProperties; //Device memory info

				//*ALL* Device queue families
				VkQueueFamilyProperties* vkQueueFamiliesArray = nullptr;
				uint32_t vkQueueFamiliesCount; //Number of queue families exposed by our selected physical device

				//Graphics queue - used for general rendering - TODO: Get queue family which is
				//capable of presentation rather than just the first available 
				//queue family that can do graphics work. 
				uint32_t vkGraphicsQueueFamilyIndex = 0; //Index in to vkQueueFamilesArray & used at queue creation time
				VkQueue vkGraphicsQueue[ENGINE_CONFIG_VULKAN_API_GRAPHICS_QUEUE_COUNT]; //All graphics processing queues

				//Memory allocation data
				//
				//index in to vkDeviceMemoryProperties.memoryTypes[]
				//
				//1) When allocating data on the GPU which is seen exclusivly
				//by said GPU (So not read/writable by CPU) -> Memory allocated
				//with this type is most efficient for device access. 
				uint32_t vkMemoryTypeIndexForEfficientDeviceOnlyAllocations = 0;
				
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

			private:
				//VK Helpers
				//
				//Caches info regarding the Vulkan device memory capabilities. 
				void CacheVKDeviceMemoryInfo();

				//From the cached memory info for the given device, these function
				//finds the best memory types to be used when allocating resources
				//
				//Returns false if no valid memory type && heap can be found
				bool FindVKMemoryTypeIndexForEfficientDeviceOnlyAllocations();
				bool FindVKMemoryTypeIndexForMappableAllocations();

				//Finds the "best" Vulkan enabled physical device for us to use
				//when creating the Vulkan physical device. Currently, it just picks the
				//first discrete GPU which should be fine for us.
				//
				//See comments for a better approach. 
				VkPhysicalDevice PickBestVulkanPhysicalDevice(VkPhysicalDevice** availPhysicalDevices,
					uint32_t availPhysicalDevicesCount);

				//Gets the queue handle for a given type. 
				//TODO: Compute etc. 
				//TODO: Queue which also supports presentation (Swapchain)???
				bool GetGraphicsQueueFamilyHandle(VkQueueFamilyProperties* deviceQueueFamiliesArray, uint32_t queueFamilyCount);
			};
		};
	};
};