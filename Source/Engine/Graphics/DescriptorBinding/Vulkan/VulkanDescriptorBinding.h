//VulkanDescriptorBinding.h
//Created 05/06/17
//Created By Daniel Bowler
//
//Descriptor sets contains an array of bindings. Each describes how one
//resource will be mapped to shaders. 

#pragma once

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Vulkan header
#include <vulkan/vulkan.h>

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanDescriptorBinding
			{
			public:
				VulkanDescriptorBinding() {};
				virtual ~VulkanDescriptorBinding() = 0 {};

				//Shutdown the descriptor object
				void Shutdown();

				//Inits the descriptor binding
				void InitVKDescriptorBindingData(uint32_t bindingIDX, VkDescriptorType descriptorType, 
					uint32_t descriptorCount, VkShaderStageFlags stageFlags, const VkSampler* immutableSamplers);

				//Gets the VkDescriptorSetLayoutBinding structure
				VkDescriptorSetLayoutBinding& GetVKDescriptorSetLayoutBindingDescription() { return vkDescriptorSetLayoutBinding; };

			protected:
				VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding = {};
			};
		};
	};
};