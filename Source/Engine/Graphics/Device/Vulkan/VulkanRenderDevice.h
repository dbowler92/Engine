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

			private:
				//Vulkan data / handles
				VkPhysicalDevice vkPhysicalDevice = NULL;
				VkDevice vkLogicalDevice = NULL;

				//Device properties
				VkPhysicalDeviceProperties vkDeviceProperties;

				//Device queue families
				VkQueueFamilyProperties* vkQueueFamiliesArray = nullptr;
				uint32_t vkQueueFamiliesCount;

				//Index to our graphics queue family????
				uint32_t vkGraphicsQueueFamilyIndex = 0;

				//Device memory info
				VkPhysicalDeviceMemoryProperties vkDeviceMemoryProperties;

			private:
				//Validates our chosen device extentions & layers (Depreciated I think. However, 
				//I will add the code in needed just incase!)- Are the ones
				//we want available to us? Again, doesnt attempt to recover in the
				//case they do not exist. 
				bool ValidateVKDeviceLayers(std::vector<const char*> *desiredDeviceLayers);
				bool ValidateVKDeviceExtentions(std::vector<const char*> *desiredDeviceExtentions);

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

				//Finds the "best" Vulkan enabled physical device for us to use
				//when creating the Vulkan physical device. Currently, it just picks the
				//first discrete GPU which should be fine for us.
				//
				//See comments for a better approach. 
				VkPhysicalDevice PickBestVulkanPhysicalDevice(VkPhysicalDevice** availPhysicalDevices,
					uint32_t availPhysicalDevicesCount);

				//Gets the queue handle for a given type. TODO: Compute etc. 
				bool GetGraphicsQueueFamilyHandle(VkQueueFamilyProperties* deviceQueueFamiliesArray, uint32_t queueFamilyCount);
			};
		};
	};
};