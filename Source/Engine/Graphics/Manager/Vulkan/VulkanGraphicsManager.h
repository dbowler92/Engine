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
				//Vulkan data / handles
				VkInstance vkInstance = NULL;
				VkPhysicalDevice vkPhysicalDevice = NULL; 
				VkDevice vkLogicalDevice = NULL;		 

			private:
				//VK Init
				//
				bool InitVKInstance(ECHAR* applicationTitle, int appVersionMajor, int appVersionMinor, int appVersionPatch);
				bool InitVKDevice(); //Logical device is created

			private:
				//VK Validations
				//
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

				//Like above, validates our chosen device extentions & layers (Depreciated I think. However, 
				//I will add the code in needed just incase!)- Are the ones
				//we want available to us? Again, doesnt attempt to recover in the
				//case they do not exist. 
				bool ValidateVKDeviceLayers(std::vector<const char*> *desiredDeviceLayers);
				bool ValidateVKDeviceExtentions(std::vector<const char*> *desiredDeviceExtentions);

				//Again, like above, this function checks to see if our device can
				//support the requested API feature set. NOTE: This likely should
				//be done before/at the same time we pick a specific device -> Thus, we
				//could reject a potential device that doesn't support the requested features
				//of our game/app/engine. 
				//
				//IMPORTANT: As VK is new, this function is pointless (all devices will
				//support the stuff we need) and doesnt actually do anything 
				//(instantly return true).
				//bool ValidateVKDeviceFeatures(VkPhysicalDeviceFeatures* requestedDeviceFeatures);

			private:
				//VK Helpers
				//
				//Finds the "best" Vulkan enabled physical device for us to use
				//when creating the Vulkan physical device. Currently, it just picks the
				//first discrete GPU which should be fine for us.
				//
				//See comments for a better approach. 
				VkPhysicalDevice PickBestVulkanPhysicalDevice(VkPhysicalDevice** availPhysicalDevices,
					uint32_t availPhysicalDevicesCount);

			private:
				//VK Shutdown
				//
				void ShutdownVKInstance();
				void ShutdownVKDevice();
			};
		};
	};
};
