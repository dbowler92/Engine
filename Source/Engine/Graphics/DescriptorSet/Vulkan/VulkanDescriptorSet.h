//VulkanDescriptorSet.h
//Created 05/06/17
//Created By Daniel Bowler
//
//Way that Vulkan maps resources to shaders - Eg: Textures and the like

#pragma once

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Device
#include "../../Device/RenderDevice.h"

//Needs to know about (a set of) bindings
#include "../../DescriptorBinding/DescriptorBinding.h"

//Vulkan header
#include <vulkan/vulkan.h>

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanDescriptorSet
			{
			public:
				VulkanDescriptorSet() {};
				virtual ~VulkanDescriptorSet() = 0 {};

				//Shutdown the descriptor object
				void Shutdown();

				//Inits the descriptor set - takes an array of the bindings
				bool InitVKDescriptorSet(EngineAPI::Graphics::RenderDevice* renderingDevice,
					EngineAPI::Graphics::DescriptorBinding* descriptorBindingsArray, uint32_t bindingsCount);

			protected:
				//Vulkan handle representing the descriptor set layout -> Describes the 
				//number and metadata of the descriptor bindings
				VkDescriptorSetLayout vkDescriptorSetLayoutHandle = VK_NULL_HANDLE;

				//Device used to create this set layout
				VkDevice cachedVKLogicalDevice = VK_NULL_HANDLE;
			};
		};
	};
};
