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
				
			};
		};
	};
};