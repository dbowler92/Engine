//EngineConfig.h
//Created 15/04/17
//Created By Daniel Bowler
//
//Used to set the state of the game engine

#pragma once

//Picks the common / base class that platform / API specific implementations will inherit from. Eg:
//class Win32OSWindow : public COMMON_IMPLEMENTATION(OSWindow)
#define COMMON_IMPLEMENTATION(TYPE) Interface::Common##TYPE

//Engine name
#define ENGINE_NAME "Real Engine 4"

//Engine version
#define ENGINE_VERSION_MAJOR 0
#define ENGINE_VERSION_MINOR 2
#define ENGINE_VERSION_PATCH 0

//Platform - not really used for now?
#define PLATFORM_IDX_WIN32 0

//Graphics APIs
#define GRAPHICS_API_IDX_VULKAN 0

//Current platform to build & grpahics API to use - this should really be set
//through a VS build configuration. But for now, this will do.  
#define ENGINE_BUILD_SETTINGS_PLATFORM PLATFORM_IDX_WIN32
#define ENGINE_BUILD_SETTINGS_GRAPHICS_API GRAPHICS_API_IDX_VULKAN

//Should we output debug info when setting up the graphics subsystem. Eg: Number
//of debug layers for vulkan etc. 
#define SHOULD_PRINT_GRAPHICS_INIT_INFO 1

//Should stop executing program on warning or error messages?
#define ENGINE_CONFIG_STOP_EXECUTION_ON_ERROR_MSG 1
#define ENGINE_CONFIG_STOP_EXECUTION_ON_WARNING_MSG 1

//
//PLATFORM
//
//So we can use #ifdef and #ifndef
#if ENGINE_BUILD_SETTINGS_PLATFORM == PLATFORM_IDX_WIN32
#define ENGINE_CONFIG_PLATFORM_WIN32

//Win32 defines and the like
#include "Win32Config.h"

#endif

//So we can use #ifdef and #ifndef
#if ENGINE_BUILD_SETTINGS_GRAPHICS_API == GRAPHICS_API_IDX_VULKAN
#define ENGINE_CONFIG_GRAPHICS_API_VULKAN

//Vulkan config - settings eg: Minimum vulkan API required
//Also, some API specific configs. Eg: Minumum supported vulkan API version
#include "VulkanConfig.h"

#endif