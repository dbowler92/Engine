//GraphicsConfig.h
//Created 04/05/17
//Created By Daniel Bowler
//
//Some shared graphics settings - for all APIs whenever they get implemented!

#pragma once

//AA
#define GRAPHICS_CONFIG_ENABLE_MSAA false
#if GRAPHICS_CONFIG_ENABLE_MSAA
#define GRAPHICS_CONFIG_MSAA_SAMPLE_COUNT 4
#endif

//Depth buffer format - created alongside swapchain
#define GRAPHICS_CONFIG_DO_CREATE_DEPTH_TEXTURE_ALONGSIDE_SWAPCHAIN true
#define GRAPHICS_CONFIG_DEPTH_TEXTURE_FORMAT DEPTH_TEXTURE_FORMAT_D24_S8
