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
				//WSI Extention

#endif
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