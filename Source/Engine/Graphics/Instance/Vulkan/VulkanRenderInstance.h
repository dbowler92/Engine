//VulkanRenderInstance.h
//Created 20/04/17
//Created By Daniel Bowler
//
//Represents & creates the Vulkan VkInstance

#pragma once

//Interface
#include "../Common/CommonRenderInstance.h"

#include <vector>

//Vulkan header
#include <vulkan\vulkan.h>

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanRenderInstance : public EngineAPI::Graphics::Interface::CommonRenderInstance
			{
			public:
				VulkanRenderInstance(){};
				virtual ~VulkanRenderInstance() = 0 {};

				//Init and shutdown
				//Override init and shutdown functions
				bool Init(EngineAPI::OS::OSWindow* osWindow, ECHAR* applicationTitle,
					int appVersionMajor, int appVersionMinor, int appVersionPatch) override;
				void Shutdown() override;

				//Returns the Vulkan instance
				VkInstance GetVKInstance() { return vkInstance; };

				//Returns a list of enabled (if vkInstance was created succesfully, anyway) instance layers
				//and instance extentions
				std::vector<const char*>* GetVKEnabledInstanceLayersList();
				std::vector<const char*>* GetVKEnabledInstanceExtentionsList();

			private:
				//Handle to the Vulkan instance we create
				VkInstance vkInstance = NULL;

#if ENGINE_CONFIG_VULKAN_API_ENABLE_VALIDATION_AND_DEBUG_REPORTING
			private:
				//Function pointer for the Vulkan debug report callback (instance) - this
				//needs to be dynamically linked through vkGetInstanceProcAddr()
				PFN_vkCreateDebugReportCallbackEXT onCreateDebugReportCallback = NULL;
				PFN_vkDestroyDebugReportCallbackEXT onDestroyDebugReportCallback = NULL;
				VkDebugReportCallbackEXT debugReportCallback = NULL;
				VkDebugReportCallbackCreateInfoEXT debugReportCreateInfo = {};
#endif
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
			
#if ENGINE_CONFIG_VULKAN_API_ENABLE_VALIDATION_AND_DEBUG_REPORTING
			private:
				//Sets up debug callbacks
				bool SetupVulkanDebugReportCallbacks();

				//Cleans up debug callbacks
				void CleanupVulkanDebugReporting();

				//Prints debug reports to console
				static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportPrintFunction(VkFlags msgFlags,
					VkDebugReportObjectTypeEXT objType, uint64_t srcObject, size_t location,
					int32_t msgCode, const char *layerPrefix, const char *msg, void *userData);
#endif
			};
		};
	};
};