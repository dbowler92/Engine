//VulkanDescriptorSet.h
//Created 05/06/17
//Created By Daniel Bowler
//
//Way that Vulkan maps resources to shaders - Eg: Textures and the like

#pragma once

//Vector
#include <vector>

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Device
#include "../../Device/RenderDevice.h"

//Needs to know about (a set of) bindings
#include "../../DescriptorBinding/DescriptorBinding.h"

//Uses descriptor pools to alloc sets out of
#include "../../DescriptorPool/DescriptorPool.h"

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
				bool InitVKDescriptorSetWithExistingDescriptorPool(EngineAPI::Graphics::RenderDevice* renderingDevice,
					EngineAPI::Graphics::DescriptorBinding* descriptorBindingsArray, uint32_t bindingsCount,
					EngineAPI::Graphics::DescriptorPool* descriptorPool);

			public:
				//Gets the filled descriptor set layout struct
				VkDescriptorSetLayout& GetVKDescriptorSetLayoutHandle() { return vkDescriptorSetLayoutHandle; };

			protected:
				//Descriptor set pool used to alloc descriptor set handles out of
				EngineAPI::Graphics::DescriptorPool descriptorPool;             //Pool created and managed internally (TODO)
				VkDescriptorPool cachedExistingDescriptorPool = VK_NULL_HANDLE; //External pool object used at init time

				//Vulkan handle representing the descriptor set layout -> Describes the 
				//number and metadata of the descriptor bindings
				VkDescriptorSetLayout vkDescriptorSetLayoutHandle = VK_NULL_HANDLE;

				//Descriptor set handle
				VkDescriptorSet vkDescriptorSetHandle = VK_NULL_HANDLE;

				//Device used to create this set layout
				VkDevice cachedVKLogicalDevice = VK_NULL_HANDLE;

			private:
				//Calculates the descriptor pool object init info from a list
				//of bindings. This is done if we init the set via InitVKDescriptorSet()
				//which manages a descriptor pool internally for this set. 
				void CalculateDescriptorPoolInitInfo(EngineAPI::Graphics::DescriptorBinding* descriptorBindingsArray, uint32_t bindingsCount,
					std::vector<VkDescriptorPoolSize>& poolSizesOut);
			};
		};
	};
};
