#include "VulkanDescriptorBinding.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanDescriptorBinding::Shutdown()
{}

void VulkanDescriptorBinding::InitVKDescriptorBindingData(uint32_t bindingIDX, VkDescriptorType descriptorType,
	uint32_t descriptorCount, VkShaderStageFlags stageFlags, const VkSampler* immutableSamplers)
{
	//Fill struct
	vkDescriptorSetLayoutBinding.binding = bindingIDX;
	vkDescriptorSetLayoutBinding.descriptorType = descriptorType;
	vkDescriptorSetLayoutBinding.descriptorCount = descriptorCount;
	vkDescriptorSetLayoutBinding.stageFlags = stageFlags;
	vkDescriptorSetLayoutBinding.pImmutableSamplers = immutableSamplers;
}