//VulkanGraphicsPipelineCache.h
//Created 27/05/17
//Created By Daniel Bowler
//
//Represents a pipeline cache object in Vulkan -> Graphics

#pragma once

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Device used to create this...
#include "../../../Graphics/Device/RenderDevice.h"

//Vulkan header
#include <vulkan/vulkan.h>

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanGraphicsPipelineCache
			{
			public:
				VulkanGraphicsPipelineCache() {};
				virtual ~VulkanGraphicsPipelineCache() = 0 {};

				//Shutsdown the pipeline object
				void Shutdown();

				//Inits the pipeline cache. NOTE: It is possible to reuse a pipeline cache. Eg: Save the 
				//state of the PCO to file and then reuse it the next time the application is ran. Or 
				//perhaps use the state of another PCO managed by the application as an initial state
				//for this PCO.
				bool InitVKPipelineCache(EngineAPI::Graphics::RenderDevice* renderingDevice);
				bool InitVKPipelineCacheWithInitialData(EngineAPI::Graphics::RenderDevice* renderingDevice, void* optionalPCOInitData, size_t optionalPCOInitDataSizeBytes);

				//Called when merging event has occurred (See: VulkanStatics)
				void OnPCOMergeEvent(bool isDestinationOfMerge);

			public:
				//Gets the VK PCO handle
				VkPipelineCache GetVKPipelineCacheHandle() { return vkPipelineCacheHandle; };

				//Returns the PCO data -> Can be written to file and reused or passed 
				//as initial data to another PCO
				bool GetPipelineCacheStateData(void* dataOut, size_t* dataSizeOut);

			protected:
				//Pipeline cache object
				VkPipelineCache vkPipelineCacheHandle = VK_NULL_HANDLE;

				//Cached device -> Owner of this cache
				VkDevice cachedVKLogicalDevice = VK_NULL_HANDLE;

			protected:
				//Did we load this PCO with initial data
				bool didLoadPCOWithInitialState = false;
			};
		};
	};
};
