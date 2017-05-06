//VulkanRenderInstance.h
//Created 20/04/17
//Created By Daniel Bowler
//
//Represents & creates the Vulkan VkInstance

#pragma once

//Globals
#include "../../../Main/EngineStd.h"

//Debug
#include "../../../Debug/Log/DebugLog.h"

//May need to know about the OS window
#include "../../../OSWindow/OSWindow.h"

#include <vector>

//Vulkan header
#include <vulkan\vulkan.h>

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanRenderInstance
			{
			public:
				VulkanRenderInstance(){};
				virtual ~VulkanRenderInstance() = 0 {};

				//Cleansup at the end of the app life cycle
				void Shutdown();

				//Inits the VK instance
				bool InitVKInstance(EngineAPI::OS::OSWindow* osWindow, ECHAR* applicationTitle,
					int appVersionMajor, int appVersionMinor, int appVersionPatch);

				//Returns the Vulkan instance
				VkInstance GetVKInstance() { return vkInstance; };

				//Returns a list of enabled (if vkInstance was created successfully, anyway) instance layers
				//and instance extensions
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
				//Validate requested instance layers and extensions. We will
				//pass a list of layers and extensions to this function that we wish
				//to enable and these functions will see if they are available for us
				//to use. 
				//
				//Currently, they will return false if the desired layers and/or extensions
				//are not available and the app should probably quit at this point. A better solution
				//would be to attempt to continue with the reduced feature set. Eg: If the 
				//Win32 surface extension is not available, the graphics subsystem would attempt
				//to find a solution to integrate with the OS window using alternative
				//techniques
				bool ValidateVKInstanceLayers(std::vector<const char*> *desiredInstanceLayers);
				bool ValidateVKInstanceExtentions(std::vector<const char*> *desiredInstanceExtentions);
			
#if ENGINE_CONFIG_VULKAN_API_ENABLE_VALIDATION_AND_DEBUG_REPORTING
			private:
				//Sets debug reporting struct
				void SetupVulkanDebugReportingInfoStruct();

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