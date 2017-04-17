//GraphicsManager.h
//Created 16/04/17
//Created By Daniel Bowler
//
//Represents the engines graphics subsystem. Will inherit from an API specifc implementation
//depending on build settings. 

#pragma once

#include "../../Config/EngineConfig.h" //Build settings

//Platform
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "Vulkan\VulkanGraphicsManager.h"
#endif
#ifdef ENGINE_CONFIG_GRAPHICS_API_D3D11
#include "D3D11\D3D11GraphicsManager.h"
#endif
//...
//...
//...
#ifdef ENGINE_CONFIG_GRAPHICS_API_GNM
#include "GNM\GNMGraphicsManager.h"
#endif

namespace EngineAPI
{
	namespace Graphics
	{
		class GraphicsManager :
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN 
			public EngineAPI::Graphics::Platform::VulkanGraphicsManager
#endif
#ifdef ENGINE_CONFIG_GRAPHICS_API_D3D11
			public EngineAPI::Graphics::Platform::D3D11GraphicsManager
#endif
			//...
			//...
			//...
#ifdef ENGINE_CONFIG_GRAPHICS_API_GNM
			public EngineAPI::Graphics::Platform::GNMGraphicsManager
#endif
		{
		public:
			GraphicsManager() {};
		};
	};
};