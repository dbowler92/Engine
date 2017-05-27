#include "VulkanGraphicsPipelineCache.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanGraphicsPipelineCache::Shutdown()
{
	if (vkPipelineCacheHandle)
	{
		vkDestroyPipelineCache(cachedVKLogicalDevice, vkPipelineCacheHandle, nullptr);
		vkPipelineCacheHandle = VK_NULL_HANDLE;
	}
}

bool VulkanGraphicsPipelineCache::InitVKPipelineCache(EngineAPI::Graphics::RenderDevice* renderingDevice)
{
	//Cache the device
	cachedVKLogicalDevice = renderingDevice->GetVKLogicalDevice();

	//PCO description
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
	pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	pipelineCacheCreateInfo.pNext = nullptr;
	pipelineCacheCreateInfo.flags = 0;
	pipelineCacheCreateInfo.initialDataSize = 0;
	pipelineCacheCreateInfo.pInitialData = nullptr;

	//Create pipeline cache
	VkResult result = vkCreatePipelineCache(cachedVKLogicalDevice, &pipelineCacheCreateInfo, nullptr, &vkPipelineCacheHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsPipelineCache::InitVKPipelineCache() Error: Could not init pipeline cache object\n");
		return false;
	}

	//Created empty
	didLoadPCOWithInitialState = false;

	//Done
	return true;
}

bool VulkanGraphicsPipelineCache::InitVKPipelineCacheWithInitialData(EngineAPI::Graphics::RenderDevice* renderingDevice, 
	void* optionalPCOInitData, size_t optionalPCOInitDataSizeBytes)
{
	//Quick checks
	assert(optionalPCOInitData != nullptr);
	assert(optionalPCOInitDataSizeBytes != 0);

	//
	//TODO: Validate incoming data using header data
	//


	//Cache the device
	cachedVKLogicalDevice = renderingDevice->GetVKLogicalDevice();

	//PCO description
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
	pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	pipelineCacheCreateInfo.pNext = nullptr;
	pipelineCacheCreateInfo.flags = 0;
	pipelineCacheCreateInfo.initialDataSize = optionalPCOInitDataSizeBytes;
	pipelineCacheCreateInfo.pInitialData = optionalPCOInitData;

	//Create pipeline cache
	VkResult result = vkCreatePipelineCache(cachedVKLogicalDevice, &pipelineCacheCreateInfo, nullptr, &vkPipelineCacheHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsPipelineCache::InitVKPipelineCacheWithInitialData() Error: Could not init pipeline cache object\n");
		return false;
	}

	//Loaded with initial data
	didLoadPCOWithInitialState = true;

	//Done
	return true;
}

void VulkanGraphicsPipelineCache::OnPCOMergeEvent(bool isDestinationOfMerge)
{
	//TODO:
}

bool VulkanGraphicsPipelineCache::GetPipelineCacheStateData(void* dataOut, size_t* dataSizeOut)
{
	VkResult result = vkGetPipelineCacheData(cachedVKLogicalDevice, vkPipelineCacheHandle, dataSizeOut, dataOut);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsPipelineCache::GetPipelineCacheData() Error when trying to retrive PCO data\n");
		return false;
	}

	//Done
	return true;
}