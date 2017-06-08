#include "VulkanSamplerState.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanSamplerState::Shutdown()
{
	if (vkSamplerStateHandle)
	{
		vkDestroySampler(cachedVKLogicalDevice, vkSamplerStateHandle, nullptr);
		vkSamplerStateHandle = VK_NULL_HANDLE;
	}
}

bool VulkanSamplerState::InitVKSamplerState(EngineAPI::Graphics::RenderDevice* renderingDevice,
	const SamplerMinMagState* minMagState, const SamplerMipmapState* mipmapState,
	const SamplerLODState* lodState, const SamplerAddressState* addressState,
	const SamplerCompareOpState* compareOp, const SamplerAnisotropyState* anisotropyState,
	VkBool32 unormalizedCoords)
{
	//Cache device
	cachedVKLogicalDevice = renderingDevice->GetVKLogicalDevice();

	//Fill out creation structure. 
	VkSamplerCreateInfo samplerCreateInfo = {};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.pNext = nullptr;
	samplerCreateInfo.flags = 0;

	samplerCreateInfo.minFilter = minMagState->MinFilter;
	samplerCreateInfo.magFilter = minMagState->MagFilter;

	samplerCreateInfo.mipmapMode = mipmapState->MinmapMode;
	samplerCreateInfo.mipLodBias = mipmapState->MipLODBias;

	samplerCreateInfo.minLod = lodState->MinLOD;
	samplerCreateInfo.maxLod = lodState->MaxLOD;

	samplerCreateInfo.addressModeU = addressState->AddressModeU;
	samplerCreateInfo.addressModeV = addressState->AddressModeV;
	samplerCreateInfo.addressModeW = addressState->AddressModeW;
	samplerCreateInfo.borderColor  = addressState->BorderColour;

	samplerCreateInfo.compareEnable = compareOp->CompareEnabled;
	samplerCreateInfo.compareOp		= compareOp->CompareOp;

	//Verify anisotropy enabled???
	samplerCreateInfo.anisotropyEnable = anisotropyState->AnisotropyEnabled;
	samplerCreateInfo.maxAnisotropy	   = anisotropyState->MaxAnisotropy;

	samplerCreateInfo.unnormalizedCoordinates = unormalizedCoords;

	//Create sampler object
	VkResult result = vkCreateSampler(cachedVKLogicalDevice, &samplerCreateInfo, nullptr, &vkSamplerStateHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanSamplerState::InitVKSamplerState() Error: Could not create VkSampler object\n");
		return false;
	}

	//Done
	return true;
}