//VulkanSwapchain.h
//Created 20/04/17
//Created By Daniel Bowler
//
//Vulkan implementation of the swapchain

#pragma once

//OS window, graphics instance and device maybe needed
//to setup the swapchain
#include "../../../OSWindow/OSWindow.h"
#include "../../Instance/RenderInstance.h"
#include "../../Device/RenderDevice.h"

//Will need to manage a depth buffer
#include "../../../Rendering/DepthTexture/DepthTexture.h"

//Manages a render pass and framebuffers
#include "../../RenderPass/RenderPass.h"
#include "../../Framebuffer/Framebuffer.h"

//TEMP: Uses DrawableObject to build render pass instance command buffers
#include "../../../Rendering/DrawableObject/DrawableObject.h"


namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanSwapchain
			{
			public:
				VulkanSwapchain() {};
				virtual ~VulkanSwapchain() = 0 {};

				//Shutdown at the end of the app
				void Shutdown(bool doShutdownLogicalSurface = true, bool doShutdownSwapchainObject = true);

				//OnResize event - TODO
				bool OnResize(EngineAPI::OS::OSWindow* osWindow,
					EngineAPI::Graphics::RenderInstance* renderingInstance, 
					EngineAPI::Graphics::RenderDevice* renderingDevice);

				//Inits the VK swapchain
				bool InitVKLogicalSurface(EngineAPI::OS::OSWindow* osWindow,
					EngineAPI::Graphics::RenderInstance* renderingInstance);
				bool InitVKSwapchain(EngineAPI::OS::OSWindow* osWindow,
					EngineAPI::Graphics::RenderInstance* renderingInstance,
					EngineAPI::Graphics::RenderDevice* renderingDevice);

				//Inits the VK framebuffers and render pass instance command buffers - This
				//should be called after a Render pass has been inited by the graphics manager
				bool InitVKFramebuffers(EngineAPI::Graphics::RenderDevice* renderingDevice,
					EngineAPI::Graphics::RenderPass* renderPass);
				
			public:
				//Called when we want to begin rendering to the swapchain images (backbuffer)
				bool GetNextSwapchainImage(EngineAPI::Graphics::RenderDevice* renderingDevice);

				//Called when we want to present the swapchain to the monitor
				bool Present(EngineAPI::Graphics::RenderDevice* renderingDevice);

			public:
				//Returns the logical surface - needed when creating device queues
				VkSurfaceKHR GetVKLogicalSurfaceKHR() { return vkSurfaceHandle; };

				//Swpachain getters
				VkFormat GetSwpachainImageFormat() { return vkSwapchainSurfaceFormat; };
				VkExtent2D GetSwapchainDimentions() { return vkSwapchainExtents; };

				VkImageView GetVKImageViewForColourBuffer(uint32_t index) { return vkSwapchainColourImageViews[index]; };
				uint32_t GetSwapchainColourBufferCount() { return vkSwapchainColourImagesCount; };

				//Gets the index of the current active swapchain image
				uint32_t GetCurrentSwapchainImageIndex() { return currentColourBufferIndex; };

				//Gets the framebuffer for colour buffer[index]
				EngineAPI::Graphics::Framebuffer* GetFramebufferObjectForSwapchainColourBuffer(uint32_t index) { return &swapchainFramebuffers[index]; };

				//Returns the depth buffer
				EngineAPI::Rendering::DepthTexture* GetDepthTexture() { return &depthTexture; };

			private:
				//WSI created logical surface
				VkSurfaceKHR vkSurfaceHandle = VK_NULL_HANDLE;

				//Swapchain handle
				VkSwapchainKHR vkSwapchainHandle = VK_NULL_HANDLE;

				//Swapchain images (colour buffers) - NOTE: We should *not* destroy these ourselves - the extention
				//owns and manages them!
				VkImage* vkSwapchainColourImages = nullptr;
				VkImageView* vkSwapchainColourImageViews = nullptr;
				uint32_t vkSwapchainColourImagesCount = 0; //Actual number of images created

				//Index in to vkSwapchainColourImages[] && vkSwpachainColourImageViews[] of the
				//currently set colour buffer that we are rendering in to. 
				uint32_t currentColourBufferIndex = 0;

				//Cached VK data
				VkInstance cachedVKInstance = VK_NULL_HANDLE;
				VkDevice cachedVKDevice = VK_NULL_HANDLE;

				//Surface capabilities info
				VkSurfaceCapabilitiesKHR surfaceCapabilities = {};

				VkSurfaceFormatKHR* surfaceFormatsArray = nullptr;
				uint32_t surfaceFormatsCount = 0;

				VkPresentModeKHR* presentationModesArray = nullptr;
				uint32_t presentationModesCount = 0;

				//Swapchain settings
				VkFormat vkSwapchainSurfaceFormat;
				VkColorSpaceKHR vkSwapchainColourBufferColourSpace;
				VkExtent2D vkSwapchainExtents;
				VkPresentModeKHR vkSwapchainPresentMode; 
				uint32_t vkSwapchainDesiredBuffersCount; 
				VkSurfaceTransformFlagBitsKHR vkSwapchainSurfacePreTransformFlags; //Transform relative to the presentation engines natural orientation

				//Syncronisation
				//
				//Bind and clear fence -> Used to wait on vkGetNextSwapchainImageKHR()
				VkFence vkGetNextImageFence = VK_NULL_HANDLE;

			private:
				//Creates and manages a depth buffer to be used alongside the
				//swapchain
				EngineAPI::Rendering::DepthTexture depthTexture;

				//Framebuffer objects - one per colour buffer
				std::vector<EngineAPI::Graphics::Framebuffer> swapchainFramebuffers;

			private:
				//WSI Extension
				//
				//Instance based:
				//PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR = VK_NULL_HANDLE; //Now used in VulkanRenderDevice -> Does queue support presentation?
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

			private:
				//Cache device and instance based extension functions
				bool CacheInstanceExtentionFunctions(VkInstance instance);
				bool CacheDeviceExtentionFunctions(VkDevice device);

				//Caches surface capabilities info for swapchain creation
				bool CacheSurfaceFormats(VkPhysicalDevice physicalDeviceHandle);
				bool CacheSurfaceCapabilities(VkPhysicalDevice physicalDeviceHandle);
				bool CacheSurfacePresentationModes(VkPhysicalDevice physicalDeviceHandle);

			private:
				//At init time, sets the initial settings of the swapchain. TODO: A system that
				//can change these settings at runtime (or post load, or as a result of the user
				//changing the settings, or as a result of an graphics.ini load)
				//
				//Selects a surface format for the swapchain - This is based on surface capabilities
				bool SelectDefaultVKSurfaceFormatForSwapchain();

				//Swapchain image extents (size) -> ideally, based on window size
				bool SelectDefaultSwapchainImageExtents(EngineAPI::OS::OSWindow* osWindow);

				//Selects an initial swapchain presentation mode -> This is based on a #define in VulkanConfig.h.
				bool SelectDefaultVKPresentationMode();

				//Selects an initial number of VKImages in the swapchain (single, double, tripple buffering) - this is
				//based on a #define in VulkanConfig.h. 
				bool SelectDefaultVKSwapchainImageCount();

				//Setsup the default pre transform
				bool SelectDefaultVKPreTransform();

			private:
				//Creates the swapchain - will destroy the old swapchain if it exists. 
				bool CreateVKSwpachain();

				//Gets the swapchain images and stores reference to them
				bool CacheSwapchainColourImages();

				//From the swapchains images, we have to create Vulkan views for them
				bool CreateVKSwapchainColourImageViews();

				//After the swapchain colour buffers have been created, we will
				//create a depth texture/buffer to be used alongside the swapchain
				//colour images
				bool CreateDepthBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice);
			};
		};
	};
};