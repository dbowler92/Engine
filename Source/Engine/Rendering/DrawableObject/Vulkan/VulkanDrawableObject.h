//VulkanDrawableObject.h
//Created 31/05/17
//Created By Daniel Bowler
//
//Object that can be rendered to the screen - manages the required vulkan
//data (eg: pipeline, cmd buffers etc). 

#pragma once

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Rendering includes
#include "../../../Includes/RenderingIncludes.h"

//Graphics manager
//#include "../../../Graphics/Manager/GraphicsManager.h"

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
				virtual void Shutdown() = 0 {};

				//Updates the object
				virtual void Update(float dt) = 0 {};

				//Called when we need to generate rendering commands. 
				virtual void GenerateRenderingCommands(EngineAPI::Graphics::GraphicsManager* graphicsSubsystem) = 0;
			};
		};
	};
};