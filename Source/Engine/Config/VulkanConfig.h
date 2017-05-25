//VulkanConfig.h
//Created 18/04/17
//Created By Daniel Bowler
//
//Some config stuff for Vulkan - removes it from EngineConfig.h which, it seems, may
//get too big for its own good!

#pragma once

//Global graphics settings
#include "GraphicsConfig.h"

//Macro which will select the correct subclass to inherit from for graphics classes
#define RENDERING_PLATFORM_IMPLEMENTATION(TYPE) Platform::##Vulkan##TYPE

//Macro which picks API specific version of the class TYPE
#define RENDERING_PLATFORM_CLASS(TYPE) Vulkan##TYPE

//Macros to return Vulkan instance and device extention functions
#define INSTANCE_FUNC_PTR(instance, entrypoint){ fp##entrypoint = (PFN_vk##entrypoint) vkGetInstanceProcAddr(instance, "vk"#entrypoint); if (fp##entrypoint == NULL){ exit(-1);} }
#define DEVICE_FUNC_PTR(dev, entrypoint){ fp##entrypoint = (PFN_vk##entrypoint)vkGetDeviceProcAddr(dev, "vk"#entrypoint); if (fp##entrypoint == NULL){exit(-1);} }

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

//Do we report info -> Currently, this will spit out (per frame) that we create and destroy
//a semaphore. This is annoying...
#define ENGINE_CONFIG_VULKAN_API_DO_PRINT_DEBUG_INFO true
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

//Graphics command buffer pool creation state
#define ENGINE_CONFIG_VULKAN_API_GRAPHICS_COMMAND_BUFFER_POOLS_ALLOW_BUFFER_RESETS true
#define ENGINE_CONFIG_VULKAN_API_GRAPHICS_COMMAND_BUFFER_POOLS_IS_TRANSIENT false

//Desired swapachain present mode
#define ENGINE_CONFIG_VULKAN_API_SWAPCHAIN_DEFAULT_PRESENT_MODE VK_PRESENT_MODE_MAILBOX_KHR
//#define ENGINE_CONFIG_VULKAN_API_SWAPCHAIN_DEFAULT_PRESENT_MODE VK_PRESENT_MODE_FIFO_RELAXED_KHR

//Desired swpachain buffer count (double buffering)
#define ENGINE_CONFIG_VULKAN_API_SWAPCHAIN_DEFAULT_BUFFER_COUNT 2

//Memory
//
//Maximum number of memory stores that we can have in the application -> Each vulkan implementation
//does have an upper limit to the number of individual memory store allocs. 
#define ENGINE_CONFIG_VULKAN_API_MAX_NUMBER_OF_MEMORY_STORES 4096

//Maximum number of memory blocks in each store (Suballocations from each store)
//#define ENGINE_CONFIG_VULKAN_API_MAX_NUMBER_OF_MEMORY_BLOCKS_PER_STORE 128

//Size of the static block of memory used to hold data that remains throughout the
//entire life of the application.
//
//NOTE: This value should be easy to work out with a proper asset pipeline - just count the 
//bytes needed for textures, resources etc all marked as having a lifespan that lasts the 
//full run of the game in a build process. This would reduce wasted memory. However, thats
//a bit outside the scope of this engine for now -> so i've reverted to a nice and simple #define!
#define ENGINE_CONFIG_VULKAN_API_GLOBAL_STATIC_MEMORY_BLOCK_SIZE_MB 32

//Size of each memory store (created when we generate a new store through the AllocResourceAuto() calls) that is DEVICE_LOCAL only
#define ENGINE_CONFIG_VULKAN_API_DEVICE_LOCAL_STORE_SIZE_MB 256

//Size of each memory store (created when we generate a new store through the AllocResourceAuto() calls) that is HOST_LOCAL
#define ENGINE_CONFIG_VULKAN_API_HOST_STORE_SIZE_MB 64
