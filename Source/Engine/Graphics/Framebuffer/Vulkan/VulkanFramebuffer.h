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
			};
		};
	};
};