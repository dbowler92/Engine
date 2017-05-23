//VulkanFramebuffer.h
//Created 23/05/17
//Created By Daniel Bowler
//
//Abstracts/wraps a vulkan framebuffer object

#pragma once

//Globals
#include "../../../Main/EngineStd.h"

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Device
#include "../../Device/RenderDevice.h"

//Vulkan static functions
#include "../../../Statics/Vulkan/VulkanStatics.h"

//Vulkan header
#include <vulkan\vulkan.h>

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanFramebuffer
			{
			public:
				VulkanFramebuffer() {};
				virtual ~VulkanFramebuffer() = 0 {};

				//Cleansup
				void Shutdown();

			public:
				//Inits the framebuffer object
				bool InitVKFramebuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
					VkFramebufferCreateInfo* framebufferCreateInfo);

			protected:
				//Cached device that owns the framebuffer
				VkDevice cachedVkDevice = VK_NULL_HANDLE;

				//Framebuffer handle
				VkFramebuffer vkFramebufferHandle = VK_NULL_HANDLE;
			};
		};
	};
};