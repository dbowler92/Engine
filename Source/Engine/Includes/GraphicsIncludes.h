//GraphicsIncludes.h
//Created 26/04/17
//Created by Daniel Bowler
//
//Simple header that will include all graphics headers required

#pragma once

#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
#include "../Graphics/Statics/Vulkan/VulkanStatics.h" //Helper / static functions
#endif

#include "../Graphics/Manager/GraphicsManager.h"
#include "../Graphics/Instance/RenderInstance.h"
#include "../Graphics/Device/RenderDevice.h"
#include "../Graphics/Swapchain/Swapchain.h"

#include "../Graphics/CommandBufferPool/CommandBufferPool.h"
#include "../Graphics/RenderCommandBuffer/RenderCommandBuffer.h"

#include "../Graphics/CommandQueueFamily/CommandQueueFamily.h"
#include "../Graphics/CommandQueue/CommandQueue.h"