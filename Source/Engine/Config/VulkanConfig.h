//VulkanConfig.h
//Created 18/04/17
//Created By Daniel Bowler
//
//Some config stuff for Vulkan - removes it from EngineConfig.h which, it seems, may
//get too big for its own good!

#pragma once

//Macro which will select the correct subclass to inherit from for graphics classes
#define RENDERING_PLATFORM_IMPLEMENTATION(TYPE) Platform::##Vulkan##TYPE

//Win32 - nice and easy way to have Vulkan interact with the Win32 OS
#ifdef ENGINE_CONFIG_PLATFORM_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

//Should we enable debug layers & extentions + debug reporting to the console thanks
//to the Vulkan debug report callbacks
#define ENGINE_CONFIG_VULKAN_API_ENABLE_VALIDATION_AND_DEBUG 1
#define ENGINE_CONFIG_VULKAN_API_ENABLE_VALIDATION_AND_DEBUG_REPORTING 1

#if ENGINE_CONFIG_VULKAN_API_ENABLE_VALIDATION_AND_DEBUG_REPORTING
//If reporting errors, should we stop the app in the case of an error report, 
//warning report and performance warning report? 
#define ENGINE_CONFIG_VULKAN_API_STOP_EXECUTION_ON_DEBUG_ERROR 1
#define ENGINE_CONFIG_VULKAN_API_STOP_EXECUTION_ON_DEBUG_WARNING 0
#define ENGINE_CONFIG_VULKAN_API_STOP_EXECUTION_ON_DEBUG_PERFORMANCE_WARNING 0
#endif

//Minimum Vulkan driver level required by this application. If the user
//doesnt have this level, the game/app will not work. Politly tell them 
//to upgrade to a brand new GPU
#define ENGINE_CONFIG_MINIMUM_VULKAN_API_MAJOR 1
#define ENGINE_CONFIG_MINIMUM_VULKAN_API_MINOR 0
#define ENGINE_CONFIG_MINIMUM_VULKAN_API_PATCH 0

//
//TODO: API feature set (VkPhysicalDeviceFeatures struct) - eg: compute, geometry
//shader, tessellation stage etc. 
//

//Queues
#define ENGINE_CONFIG_VULKAN_API_GRAPHICS_QUEUE_COUNT 1
#define ENGINE_CONFIG_VULKAN_API_COMPUTE_QUEUE_COUNT  0  //Compute
#define ENGINE_CONFIG_VULKAN_API_TRANSFER_QUEUE_COUNT 0  //DMA
#define ENGINE_CONFIG_VULKAN_API_SPARSE_QUEUE_COUNT   0  //Spare memory

//Number of command buffer pools that we should created
#define ENGINE_CONFIG_VULKAN_API_GRAPHICS_COMMAND_BUFFER_POOLS_COUNT 1