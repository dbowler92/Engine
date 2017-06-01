//VulkanDrawableObject.h
//Created 31/05/17
//Created By Daniel Bowler
//
//Object that can be rendered to the screen - manages the required vulkan
//data (eg: pipeline, cmd buffers etc). 

#pragma once

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Vulkan header
#include <vulkan/vulkan.h>

namespace EngineAPI
{
	namespace Graphics
	{
		class GraphicsManager;
	};
};

namespace EngineAPI
{
	namespace Rendering
	{
		namespace Platform
		{
			class VulkanDrawableObject
			{
			public:
				VulkanDrawableObject() {};
				virtual ~VulkanDrawableObject() = 0 {};
			
				//Shutdown this object
				void Shutdown();

				//Inits the drawable object
				bool Init(EngineAPI::Graphics::GraphicsManager* graphicsManager);

				//Called when we need to 
			};
		};
	};
};