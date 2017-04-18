//VulkanConfig.h
//Created 18/04/17
//Created By Daniel Bowler
//
//Some config stuff for Vulkan - removes it from EngineConfig.h which, it seems, may
//get too big for its own good!

#pragma once

//Win32 - nice and easy way to have Vulkan interact with the Win32 OS
#ifdef ENGINE_CONFIG_PLATFORM_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

//Minimum Vulkan driver level required by this application. If the user
//doesnt have this level, the game/app will not work. Politly tell them 
//to upgrade to a brand new GPU
#define ENGINE_CONFIG_MINIMUM_VULKAN_API_MAJOR 1
#define ENGINE_CONFIG_MINIMUM_VULKAN_API_MINOR 0
#define ENGINE_CONFIG_MINIMUM_VULKAN_API_PATCH 0

//Should we enable debug layers & extentions (instance)
#define ENGINE_CONFIG_VULKAN_API_ENABLE_VALIDATION_AND_DEBUG_LAYERS_ON_INSTANCE 1
#define ENGINE_CONFIG_VULKAN_API_ENABLE_VALIDATION_AND_DEBUG_EXTENTIONS_ON_INSTANCE 1
