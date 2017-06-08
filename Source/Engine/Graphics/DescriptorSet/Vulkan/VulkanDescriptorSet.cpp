#include "VulkanDescriptorSet.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanDescriptorSet::Shutdown()
{
	//Writes
	if (vkWriteDescriptorSetArray)
		delete[] vkWriteDescriptorSetArray;

	//Sets
	if (vkDescriptorSetHandle != VK_NULL_HANDLE)
	{
		//Internally managed pool was used
		if (descriptorPool.GetVKDescriptorPoolHandle() != VK_NULL_HANDLE)
		{
			vkFreeDescriptorSets(cachedVKLogicalDevice, descriptorPool.GetVKDescriptorPoolHandle(), 1, &vkDescriptorSetHandle);
			descriptorPool.Shutdown();
		}

		//External pool was used
		if (cachedExistingDescriptorPool != VK_NULL_HANDLE)
			vkFreeDescriptorSets(cachedVKLogicalDevice, cachedExistingDescriptorPool, 1, &vkDescriptorSetHandle);
	}

	//Layout
	if (vkDescriptorSetLayoutHandle)
	{
		vkDestroyDescriptorSetLayout(cachedVKLogicalDevice, vkDescriptorSetLayoutHandle, nullptr);
		vkDescriptorSetLayoutHandle = VK_NULL_HANDLE;
	}
}

bool VulkanDescriptorSet::InitVKDescriptorSet(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Graphics::DescriptorBinding* descriptorBindingsArray, uint32_t bindingsCount)
{
	//Cache device
	cachedVKLogicalDevice = renderingDevice->GetVKLogicalDevice();

	//Number of bindings. 
	this->bindingsCount = bindingsCount;

	//Create array of VkDescriptorSetLayoutBindings -> One for each input binding
	std::vector<VkDescriptorSetLayoutBinding> layoutBidings(bindingsCount);
	for (int i = 0; i < bindingsCount; ++i)
		layoutBidings[i] = descriptorBindingsArray[i].GetVKDescriptorSetLayoutBindingDescription();

	//Create layout
	VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutCreateInfo.pNext = nullptr;
	layoutCreateInfo.flags = 0;
	layoutCreateInfo.pBindings = bindingsCount > 0 ? layoutBidings.data() : nullptr;
	layoutCreateInfo.bindingCount = bindingsCount;

	VkResult result = vkCreateDescriptorSetLayout(cachedVKLogicalDevice, &layoutCreateInfo, nullptr, &vkDescriptorSetLayoutHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDescriptorSet::InitVKDescriptorSet() Error: Could not create descriptor set layout\n");
		return false;
	}

	//Create descriptor pool object
	//
	//First, we need to know how many samplers, uniform buffers etc need to be created
	std::vector<VkDescriptorPoolSize> pools;
	CalculateDescriptorPoolInitInfo(descriptorBindingsArray, bindingsCount, pools);

	if (!descriptorPool.InitVKDescriptorPools(renderingDevice, pools.data(), pools.size(), true))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDescriptorSet::InitVKDescriptorSet() Error: Could not create descriptor pool\n");
		return false;
	}

	//Create descriptor set
	VkDescriptorSetAllocateInfo setAllocInfo = {};
	setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	setAllocInfo.pNext = nullptr;
	setAllocInfo.pSetLayouts = &vkDescriptorSetLayoutHandle;
	setAllocInfo.descriptorSetCount = 1;
	setAllocInfo.descriptorPool = descriptorPool.GetVKDescriptorPoolHandle();

	result = vkAllocateDescriptorSets(cachedVKLogicalDevice, &setAllocInfo, &vkDescriptorSetHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDescriptorSet::InitVKDescriptorSet() Error: Could not create descriptor sets\n");
		return false;
	}

	//Alloc writes descriptor
	vkWriteDescriptorSetArray = GE_NEW VkWriteDescriptorSet[bindingsCount];

	//Init
	for (int i = 0; i < bindingsCount; i++)
	{
		vkWriteDescriptorSetArray[i] = {};
		vkWriteDescriptorSetArray[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		vkWriteDescriptorSetArray[i].pNext = nullptr;
		vkWriteDescriptorSetArray[i].dstSet = vkDescriptorSetHandle;
		vkWriteDescriptorSetArray[i].descriptorCount = descriptorBindingsArray[i].GetVKDescriptorSetLayoutBindingDescription().descriptorCount;
		vkWriteDescriptorSetArray[i].descriptorType = descriptorBindingsArray[i].GetVKDescriptorSetLayoutBindingDescription().descriptorType;
		vkWriteDescriptorSetArray[i].dstBinding = descriptorBindingsArray[i].GetVKDescriptorSetLayoutBindingDescription().binding;
		vkWriteDescriptorSetArray[i].dstArrayElement = 0;
	}

	//Done
	return true;
}

bool VulkanDescriptorSet::InitVKDescriptorSetWithExistingDescriptorPool(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Graphics::DescriptorBinding* descriptorBindingsArray, uint32_t bindingsCount,
	EngineAPI::Graphics::DescriptorPool* descriptorPool)
{
	//Cache device
	cachedVKLogicalDevice = renderingDevice->GetVKLogicalDevice();

	//Cache the descriptor pool handle used to alloc sets out of
	cachedExistingDescriptorPool = descriptorPool->GetVKDescriptorPoolHandle();

	//Number of bindings
	this->bindingsCount = bindingsCount;

	//Create array of VkDescriptorSetLayoutBindings -> One for each input binding
	std::vector<VkDescriptorSetLayoutBinding> layoutBidings(bindingsCount);
	for (int i = 0; i < bindingsCount; ++i)
		layoutBidings[i] = descriptorBindingsArray[i].GetVKDescriptorSetLayoutBindingDescription();

	//Create layout
	VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutCreateInfo.pNext = nullptr;
	layoutCreateInfo.flags = 0;
	layoutCreateInfo.pBindings = bindingsCount > 0 ? layoutBidings.data() : nullptr;
	layoutCreateInfo.bindingCount = bindingsCount;

	VkResult result = vkCreateDescriptorSetLayout(cachedVKLogicalDevice, &layoutCreateInfo, nullptr, &vkDescriptorSetLayoutHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDescriptorSet::InitVKDescriptorSetWithExistingDescriptorPool() Error: Could not create descriptor set layout\n");
		return false;
	}
	
	//Alloc descriptor set
	VkDescriptorSetAllocateInfo setAllocInfo = {};
	setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	setAllocInfo.pNext = nullptr;
	setAllocInfo.pSetLayouts = &vkDescriptorSetLayoutHandle;
	setAllocInfo.descriptorSetCount = 1;
	setAllocInfo.descriptorPool = cachedExistingDescriptorPool;

	result = vkAllocateDescriptorSets(cachedVKLogicalDevice, &setAllocInfo, &vkDescriptorSetHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDescriptorSet::InitVKDescriptorSetWithExistingDescriptorPool() Error: Could not create descriptor sets\n");
		return false;
	}

	//Alloc writes descriptor
	vkWriteDescriptorSetArray = GE_NEW VkWriteDescriptorSet[bindingsCount];
	for (int i = 0; i < bindingsCount; i++)
	{
		vkWriteDescriptorSetArray[i] = {};
		vkWriteDescriptorSetArray[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		vkWriteDescriptorSetArray[i].pNext = nullptr;
		vkWriteDescriptorSetArray[i].dstSet = vkDescriptorSetHandle;
		vkWriteDescriptorSetArray[i].descriptorCount = descriptorBindingsArray[i].GetVKDescriptorSetLayoutBindingDescription().descriptorCount;
		vkWriteDescriptorSetArray[i].descriptorType = descriptorBindingsArray[i].GetVKDescriptorSetLayoutBindingDescription().descriptorType;
		vkWriteDescriptorSetArray[i].dstBinding = descriptorBindingsArray[i].GetVKDescriptorSetLayoutBindingDescription().binding;
		vkWriteDescriptorSetArray[i].dstArrayElement = 0;
	}

	//Done
	return true;
}

bool VulkanDescriptorSet::UpdateVKDescriptorSet(DescriptorSetWriteUpdateData* writeDataArray, uint32_t writeDataCount)
{
	assert(writeDataArray);
	assert(writeDataCount <= bindingsCount);

	for (int i = 0; i < writeDataCount; i++)
	{
		vkWriteDescriptorSetArray[i].descriptorCount = writeDataArray[i].DescriptorCount;
		vkWriteDescriptorSetArray[i].descriptorType = writeDataArray[i].Type;
		vkWriteDescriptorSetArray[i].dstBinding = writeDataArray[i].BindingIndex;
		vkWriteDescriptorSetArray[i].pBufferInfo = writeDataArray[i].Buffers;
		vkWriteDescriptorSetArray[i].pImageInfo = writeDataArray[i].Images;
		vkWriteDescriptorSetArray[i].pTexelBufferView = writeDataArray[i].TexelBufferViews;
	}

	//Update descriptor set
	vkUpdateDescriptorSets(cachedVKLogicalDevice, writeDataCount, &vkWriteDescriptorSetArray[0], 0, nullptr);

	//Done
	return true;
}

void VulkanDescriptorSet::CalculateDescriptorPoolInitInfo(EngineAPI::Graphics::DescriptorBinding* descriptorBindingsArray, uint32_t bindingsCount,
	std::vector<VkDescriptorPoolSize>& poolSizesOut)
{
	for (int i = 0; i < bindingsCount; i++)
	{
		bool bindingHandled = false;
		EngineAPI::Graphics::DescriptorBinding& binding = descriptorBindingsArray[i];

		//Does vector already contain an entry for this type of resource?
		for (int j = 0; j < poolSizesOut.size(); j++)
		{
			if (poolSizesOut[j].type == binding.GetVKDescriptorSetLayoutBindingDescription().descriptorType)
			{
				//Yes, increment the count of resources of this type that the pool should be allocated
				//with.
				assert(binding.GetVKDescriptorSetLayoutBindingDescription().descriptorCount == 1); //If this is not 1, we need to do some refactoring...

				poolSizesOut[j].descriptorCount += binding.GetVKDescriptorSetLayoutBindingDescription().descriptorCount;
				bindingHandled = true; 
			}
		}

		//New pool size data to add to the vector
		if (!bindingHandled)
		{
			VkDescriptorPoolSize newSizeData;
			newSizeData.type = binding.GetVKDescriptorSetLayoutBindingDescription().descriptorType;
			newSizeData.descriptorCount = 1;
			poolSizesOut.push_back(newSizeData);
		}
	}
}