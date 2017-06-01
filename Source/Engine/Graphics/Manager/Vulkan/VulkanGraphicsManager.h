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

//Manages a render pass + instance(s) and framebuffer
#include "../../Framebuffer/Framebuffer.h"
#include "../../RenderPass/RenderPass.h"
#include "../../RenderPassInstance/RenderPassInstance.h"

//Vector
#include <vector>

//Vulkan header. 
#include <vulkan\vulkan.h>

//Forward declarations
namespace EngineAPI
{
	namespace Core
	{
		namespace Platform
		{
			class Win32Application;
		};
	};
};

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanGraphicsManager
			{
				//Only application should be able to create/destroy this
				//subsystem + pass events. 
				friend EngineAPI::Core::Platform::Win32Application;

			protected:
				VulkanGraphicsManager();
				virtual ~VulkanGraphicsManager() = 0 {};

				//Inits the graphics subsystem / manager (VK)
				bool InitSubsystem(EngineAPI::OS::OSWindow* osWindow,
					ECHAR* appTitle, int appVersionMajor, int appVersionMinor, int appVersionPatch,
					unsigned screenWidth, unsigned screenHeight);

				//Shutsdown the graphics manager (VK)
				bool ShutdownSubsystem();

				//Called when the window is resized
				bool OnResize(EngineAPI::OS::OSWindow* osWindow);

				//Called when a frame begins & when a frame has ended
				void OnFrameBegin();
				void OnFrameEnd();

			public:
				//Returns the instance, device and swapchains (pointer)
				RenderInstance* GetRenderingInstance() { return &renderingInstance; };
				RenderDevice* GetRenderingDevice() { return &renderingDevice; };
				Swapchain* GetRenderingSwapchain() { return &renderingSwapchain; };

				//Gets the main render pass
				RenderPass* GetRenderPass() { return &swapchainRenderPass; };

				//Gets the current render pass instance
				RenderPassInstance* GetCurrentRenderPassInstance() { return &renderPassInstancesArray[renderingSwapchain.GetCurrentSwapchainImageIndex()]; };

				//Gets all render pass instances -> One per swapchain colour buffer
				RenderPassInstance* GetRenderPassInstances() { return renderPassInstancesArray; };
				uint32_t GetRenderPassInstancesCount() { return renderPassInstanceCount; };

			public:
				//Sets the clear values
				bool SetSwapchainClearValues(UNorm32Colour colourBufferClear, float depthClear, uint32_t stencilClear);

			protected:
				//Instance, device and swapchain handlers
				RenderInstance renderingInstance;
				RenderDevice renderingDevice;
				Swapchain renderingSwapchain;

			protected:
				//Render pass
				RenderPass swapchainRenderPass;

				//Render pass instances -> One per swapchain colour image
				RenderPassInstance* renderPassInstancesArray = nullptr;
				uint32_t renderPassInstanceCount = 0;

			protected:
				//Clear colour
				UNorm32Colour swpachainClearColour;
				float swapchainDepthClearValue = 1.0f;
				uint32_t swapchainStencilClearValue = 0;

			private:
				//Inits the render pass for this engine
				bool InitVKRenderPass();

				//Inits the render pass instances
				bool InitVKRenderPassInstances();
			};
		};
	};
};
