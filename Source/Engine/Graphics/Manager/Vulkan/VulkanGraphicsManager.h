//VulkanGraphicsManager.h
//Created 16/04/17
//Created By Daniel Bowler
//
//Represents the Vulkan API implementation of the common graphics manager. Sets up Vulkan
//for us and interacts with the OS window provided
//
//Vulkan works on Win32, Android* and Linux** as of now.
//
//*Not supported nor planned. 
//**Not supported... but I may add Linux support in the future rather than (or in addition too)
//the PS4 to show off multiplatform nature of this engine (should look good on my CV). 

#pragma once

//Shared interface
#include "../Common/CommonGraphicsManager.h"

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
			class VulkanGraphicsManager : public EngineAPI::Graphics::Interface::CommonGraphicsManager
			{
			public:
				VulkanGraphicsManager() {};
				virtual ~VulkanGraphicsManager() = 0 {};

				//Inits the graphics subsystem / manager (VK)
				bool InitSubsystem(EngineAPI::OS::OSWindow* osWindow,
					ECHAR* appTitle, int appVersionMajor, int appVersionMinor, int appVersionPatch,
					unsigned screenWidth, unsigned screenHeight) override;

				//Shutsdown the graphics manager (VK)
				bool ShutdownSubsystem() override;
			};
		};
	};
};
