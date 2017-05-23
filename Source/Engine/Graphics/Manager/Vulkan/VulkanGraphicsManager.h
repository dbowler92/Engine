//VulkanGraphicsManager.h
//Created 16/04/17
//Created By Daniel Bowler
//
//Represents the Vulkan API implementation of the common graphics manager. Sets up Vulkan
//for us and interacts with the OS window provided
//
//Vulkan works on Win32, Android* and Linux* as of now.
//
//*Not supported nor planned in this engine as of now

#pragma once

//Globals
#include "../../../Main/EngineStd.h"

//Debug
#include "../../../Debug/Log/DebugLog.h"

//OS window info - so the graphics API can talk to the OS windowing system.
#include "../../../OSWindow/OSWindow.h"

//Abstractions of rendering API
#include "../../Instance/RenderInstance.h"
#include "../../Device/RenderDevice.h"
#include "../../Swapchain/Swapchain.h"

//Manages a render pass and framebuffer
#include "../../RenderPass/RenderPass.h"
#include "../../Framebuffer/Framebuffer.h"

//Vector
#include <vector>

//Vulkan header. 
#include <vulkan\vulkan.h>

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanGraphicsManager
			{
			public:
				VulkanGraphicsManager() {};
				virtual ~VulkanGraphicsManager() = 0 {};

				//Inits the graphics subsystem / manager (VK)
				bool InitSubsystem(EngineAPI::OS::OSWindow* osWindow,
					ECHAR* appTitle, int appVersionMajor, int appVersionMinor, int appVersionPatch,
					unsigned screenWidth, unsigned screenHeight);

				//Shutsdown the graphics manager (VK)
				bool ShutdownSubsystem();

				//Returns the instance, device and swapchains (pointer)
				RenderInstance* GetRenderingInstance() { return &renderingInstance; };
				RenderDevice* GetRenderingDevice() { return &renderingDevice; };
				Swapchain* GetRenderingSwapchain() { return &renderingSwapchain; };

			public:
				//Called when the window is resized
				void OnResize(uint32_t newScreenWidth, uint32_t newScreenHeight);

				//Called when we start rendering
				void BeginFrame();

				//Called when we have finished rendering for this frame. 
				void EndFrame();

			protected:
				//Instance, device and swapchain handlers
				RenderInstance renderingInstance;
				RenderDevice renderingDevice;
				Swapchain renderingSwapchain;

			protected:
				//Render pass
				RenderPass swapchainRenderPass;

				//Framebuffers - one for each swapchain colour buffer. Depenedent on 
				//swapchainRenderPass.
				std::vector<Framebuffer> swapchainFramebuffers;

			private:
				//Inits the render pass for this engine
				bool InitVKRenderPass();

				//Inits the swapchain frame buffers
				bool InitVKSwapchainFramebuffers(unsigned screenWidth, unsigned screenHeight);
			};
		};
	};
};
