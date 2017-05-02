//VulkanSwapchain.h
//Created 20/04/17
//Created By Daniel Bowler
//
//Vulkan implementation of the swapchain

#pragma once

//Base / interface
#include "../Common/CommonSwapchain.h"

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanSwapchain : public EngineAPI::Graphics::Interface::CommonSwapchain
			{
			public:
				VulkanSwapchain() {};
				virtual ~VulkanSwapchain() = 0 {};

				//Override init and shutdown functions
				bool Init(EngineAPI::OS::OSWindow* osWindow,
					EngineAPI::Graphics::RenderInstance* renderingInstance,
					EngineAPI::Graphics::RenderDevice* renderingDevice) override;
				void Shutdown() override;
				
			private:
				//Platform specific data: Eg: Surface/swapchain creation function pointers
#ifdef ENGINE_CONFIG_PLATFORM_WIN32
				//WSI Extension
				//
				//Instance based:
				//PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR = VK_NULL_HANDLE;
				PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR = VK_NULL_HANDLE;
				PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR = VK_NULL_HANDLE;
				PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR = VK_NULL_HANDLE;
				PFN_vkDestroySurfaceKHR fpDestroySurfaceKHR = VK_NULL_HANDLE;

				//Device based:
				PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR = VK_NULL_HANDLE;
				PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR = VK_NULL_HANDLE;
				PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR = VK_NULL_HANDLE;
				PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR = VK_NULL_HANDLE;
				PFN_vkQueuePresentKHR fpQueuePresentKHR = VK_NULL_HANDLE;
#endif
			private:
				//WSI created logical surface
				VkSurfaceKHR vkSurfaceHandle = VK_NULL_HANDLE;

				//Cached VK data
				VkInstance cachedVKInstance = VK_NULL_HANDLE;
				VkDevice cachedVKDevice = VK_NULL_HANDLE;

			private:
				//Platform specific init: Vulkan supports (to date): Windows, Linux and Android
#ifdef ENGINE_CONFIG_PLATFORM_WIN32
				bool InitWin32(EngineAPI::OS::OSWindow* osWindow,
					EngineAPI::Graphics::RenderInstance* renderingInstance,
					EngineAPI::Graphics::RenderDevice* renderingDevice);
#endif
			private:
				//Platform specific shutdown: Vulkan supports (to date): Windows, Linux and Android
#ifdef ENGINE_CONFIG_PLATFORM_WIN32
				void ShutdownWin32();
#endif
			};
		};
	};
};