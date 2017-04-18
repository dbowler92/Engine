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

				//Inits the graphics subsystem / manager (VK)
				bool InitSubsystem(ECHAR* appTitle, 
					int appVersionMajor, int appVersionMinor, int appVersionPatch,
					unsigned screenWidth, unsigned screenHeight) override;

				//Shutsdown the graphics manager (VK)
				bool ShutdownSubsystem() override;

			private:
				//Vulkan data
				VkInstance vkInstance = nullptr;
				VkDevice vkDevice = nullptr; //Logical device

			private:
				//Vulkan init code
				bool InitVKInstance(ECHAR* applicationTitle, int appVersionMajor, int appVersionMinor, int appVersionPatch);
				bool InitVKDevice(); //Logical device is created

			private:
				//Validate requested instance layers and extentions. We will
				//pass a list of layers and extentions to this function that we wish
				//to enable and these functions will see if they are available for us
				//to use. 
				//
				//Currently, they will return false if the desired layers and/or extentions
				//are not available and the app should probably quit at this point. A better solution
				//would be to attempt to continue with the reduced feature set. Eg: If the 
				//Win32 surface extention is not available, the graphics subsystem would attempt
				//to find a solution to intergrate with the OS window using alternative
				//techniques
				bool ValidateVKInstanceLayers(std::vector<const char*> *desiredInstanceLayers);
				bool ValidateVKInstanceExtentions(std::vector<const char*> *desiredInstanceExtentions);

			private:
				//Vulkan deinit / shutdown code
				void ShutdownVKInstance();
				void ShutdownVKDevice();
			};
		};
	};
};
