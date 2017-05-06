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
				RenderInstance* GetRenderingInstance() { return renderingInstance; };
				RenderDevice* GetRenderingDevice() { return renderingDevice; };
				Swapchain* GetRenderingSwapchain() { return renderingSwapchain; };

			protected:
				//Instance, device and swapchain handlers
				RenderInstance* renderingInstance = nullptr;
				RenderDevice* renderingDevice = nullptr;
				Swapchain* renderingSwapchain = nullptr;
			};
		};
	};
};
