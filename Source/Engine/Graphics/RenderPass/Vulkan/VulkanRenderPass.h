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
			};
		};
	};
};
