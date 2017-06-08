//VulkanSamplerState.h
//Created 08/06/17
//Created By Daniel Bowler
//
//Info on how to sample a texture in a shader. Wrapper for VkSampler

#pragma once

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Device used to create this...
#include "../../../Graphics/Device/RenderDevice.h"

//Vulkan header
#include <vulkan/vulkan.h>

//Sampler state structs -> Rather than the init function taking a billion params, I thought it 
//would look sexier if we split this up in to a few separate structs in a logical way. Plus, this
//enables me to provide a simple way of providing default settings!
struct SamplerMinMagState
{
	SamplerMinMagState()
	{
		MagFilter = VK_FILTER_LINEAR;
		MinFilter = VK_FILTER_LINEAR;
	}

	VkFilter MagFilter;
	VkFilter MinFilter;
};

struct SamplerMipmapState
{
	SamplerMipmapState()
	{
		MinmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		MipLODBias = 0.0f;
	}

	VkSamplerMipmapMode MinmapMode;
	float MipLODBias;
};

struct SamplerLODState
{
	SamplerLODState()
	{
		MinLOD = 0.0f;
		MaxLOD = 0.0f;
	}

	float MinLOD;
	float MaxLOD; //Set to mipLevels if using optimal tiling. Linear doesnt usually support mip-mapping. 
};

struct SamplerAddressState
{
	SamplerAddressState()
	{
		AddressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		AddressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		AddressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		BorderColour = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	}

	VkSamplerAddressMode AddressModeU;
	VkSamplerAddressMode AddressModeV;
	VkSamplerAddressMode AddressModeW;
	VkBorderColor BorderColour;
};

struct SamplerCompareOpState
{
	SamplerCompareOpState()
	{
		CompareEnabled = VK_FALSE;
		CompareOp = VK_COMPARE_OP_NEVER;
	}

	VkBool32	CompareEnabled;
	VkCompareOp CompareOp;
};

struct SamplerAnisotropyState
{
	//
	//Anisotropy filtering needs to be enabled (device features) at device init time. Thus, this
	//struct defaults to off. 
	//
	SamplerAnisotropyState()
	{
		AnisotropyEnabled = VK_FALSE;
		MaxAnisotropy = 1.0f;
	}

	VkBool32 AnisotropyEnabled;
	float    MaxAnisotropy;
};

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanSamplerState
			{
			public:
				VulkanSamplerState() {};
				virtual ~VulkanSamplerState() = 0 {};

				//Shutdown the sampler state
				void Shutdown();

				//Inits the sampler state
				bool InitVKSamplerState(EngineAPI::Graphics::RenderDevice* renderingDevice, 
					const SamplerMinMagState* minMagState, const SamplerMipmapState* mipmapState, 
					const SamplerLODState* lodState, const SamplerAddressState* addressState, 
					const SamplerCompareOpState* compareOp, const SamplerAnisotropyState* anisotropyState, 
					VkBool32 unormalizedCoords);

			public:
				//Returns the sampler state handle
				VkSampler GetVKSamplerStateHandle() { return vkSamplerStateHandle; };

			protected:
				//Vulkan handle of the sampler state object
				VkSampler vkSamplerStateHandle = VK_NULL_HANDLE;

				//Device that created the sampler state
				VkDevice cachedVKLogicalDevice = VK_NULL_HANDLE;
			};
		};
	};
};