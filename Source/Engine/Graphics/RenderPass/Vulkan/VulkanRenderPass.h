//VulkanRenderPass.h
//Created 23/05/17
//Created By Daniel Bowler
//
//Abstracts/wraps a vulkan render pass object

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
			class VulkanRenderPass
			{
			public:
				VulkanRenderPass() {};
				virtual ~VulkanRenderPass() = 0 {};

				//Cleansup
				void Shutdown();

			public:
				//Inits the render pass object
				bool InitVKRenderPass(EngineAPI::Graphics::RenderDevice* renderingDevice, 
					VkRenderPassCreateInfo* renderPassCreateInfo);

				//Granularity of this render pass. 
				VkExtent2D CalculateRenderPassAreaGranularity();

			public:
				//Getters
				VkRenderPass GetVKRenderPassHandle() { return vkRenderPassHandle; };
			
			protected:
				//Device used to create the render pass
				VkDevice cachedVkDevice = VK_NULL_HANDLE;

				//Render pass object
				VkRenderPass vkRenderPassHandle = VK_NULL_HANDLE;
			};
		};
	};
};
