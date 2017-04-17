//EngineConfig.h
//Created 15/04/17
//Created By Daniel Bowler
//
//Used to set the state of the game engine

#pragma once

//Engine name
#define ENGINE_NAME "Real Engine 4"

//Engine version
#define ENGINE_VERSION_MAJOR 0
#define ENGINE_VERSION_MINOR 2
#define ENGINE_VERSION_PATCH 0

//Platform - not really used for now?
#define PLATFORM_IDX_WIN32 0
#define PLATFORM_IDX_ORBIS 1  //PS4

//Graphics APIs
#define GRAPHICS_API_IDX_VULKAN 0
#define GRAPHICS_API_IDX_D3D11 1
#define GRAPHICS_API_IDX_D3D12 2
#define GRAPHICS_API_IDX_OPENGL 3
#define GRAPHICS_API_IDX_GNM 4 //PS4

//Current platform to build & grpahics API to use - this should really be set
//through a VS build configuration. But for now, this will do.  
#define ENGINE_BUILD_SETTINGS_PLATFORM PLATFORM_IDX_WIN32
#define ENGINE_BUILD_SETTINGS_GRAPHICS_API GRAPHICS_API_IDX_VULKAN

//
//
//
//So we can use #ifdef and #ifndef
#if ENGINE_BUILD_SETTINGS_PLATFORM == PLATFORM_IDX_WIN32
#define ENGINE_CONFIG_PLATFORM_WIN32
#endif
#if ENGINE_BUILD_SETTINGS_PLATFORM == PLATFORM_IDX_ORBIS
#define ENGINE_CONFIG_PLATFORM_ORBIS
#endif

//So we can use #ifdef and #ifndef
//
//Also, some API specific configs. Eg: Minumum supported vulkan API version
#if ENGINE_BUILD_SETTINGS_GRAPHICS_API == GRAPHICS_API_IDX_VULKAN
#define ENGINE_CONFIG_GRAPHICS_API_VULKAN

#define ENGINE_CONFIG_MINIMUM_VULKAN_API_MAJOR 1
#define ENGINE_CONFIG_MINIMUM_VULKAN_API_MINOR 0
#define ENGINE_CONFIG_MINIMUM_VULKAN_API_PATCH 0

#endif
#if ENGINE_BUILD_SETTINGS_GRAPHICS_API == GRAPHICS_API_IDX_D3D11
#define ENGINE_CONFIG_GRAPHICS_API_D3D11
#endif
#if ENGINE_BUILD_SETTINGS_GRAPHICS_API == GRAPHICS_API_IDX_D3D12
#define ENGINE_CONFIG_GRAPHICS_API_D3D12
#endif
#if ENGINE_BUILD_SETTINGS_GRAPHICS_API == GRAPHICS_API_IDX_OPENGL
#define ENGINE_CONFIG_GRAPHICS_API_OPENGL
#endif
#if ENGINE_BUILD_SETTINGS_GRAPHICS_API == GRAPHICS_API_IDX_GNM
#define ENGINE_CONFIG_GRAPHICS_API_GNM
#endif

//Win32 lean and mean
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#endif

//Should we output debug info when setting up the graphics subsystem. Eg: Number
//of debug layers for vulkan etc. 
#define SHOULD_PRINT_GRAPHICS_INIT_INFO 1