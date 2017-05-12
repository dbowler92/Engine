#include "VulkanDeviceMemoryAllocator.h"

//For debug dump
#include <fstream>

//Resources
#include "../../../Rendering/DepthTexture/DepthTexture.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanDeviceMemoryAllocator::Shutdown()
{
	//Cleanup all stores
	for (int i = 0; i < ENGINE_CONFIG_VULKAN_API_MAX_NUMBER_OF_MEMORY_STORES; i++)
		deviceMemoryStoresArray[i].Shutdown();
}

bool VulkanDeviceMemoryAllocator::VulkanDeviceMemoryAllocator::InitVKMemoryAllocator()
{
	return true;
}

void VulkanDeviceMemoryAllocator::Debug_LongDump(std::string filename)
{
	//Used to write to file
	std::ofstream out;
	out.open(filename + ".DUMP");
	if (out.is_open())
	{
		out << "**************VulkanDeviceMemoryAllocator Dump****************" << "\n";
		out << "\n";
		out << "Allocator address: " << (void*)this << "\n";
		out << "Number of memory stores: " << deviceMemoryStoresCount << "\n";
		out << "\n";
		out << "**************************************************************" << "\n";
		out << "\n";

		//For each store, print info
		for (int i = 0; i < deviceMemoryStoresCount; i++)
		{
			EngineAPI::Graphics::DeviceMemoryStore& store = deviceMemoryStoresArray[i];
			
			out << "[Store: " << i << "]" << " " << (void*)&deviceMemoryStoresArray[i] << "\n";

			out << "Is Active Store: " << store.IsPublicMemoryStore() << "\n";
			out << "Is Public Store: " << store.IsPublicMemoryStore() << "\n";

			out << "Store Size: " << store.GetMemoryStoreSizeBytes() << "\n";
			out << "Vulkan Device Memory Handle: " << store.GetVKDeviceMemoryHandle() << "\n";
			out << "Vulkan Memory Type Index: " << store.GetVKMemoryTypeIndex() << "\n";
			out << "Vulkan Memory Property Flags: " << store.GetStoreMemoryPropertyFlags() << "\n";
			out << "Is Memory Mappable: " << store.IsVKMemoryMappable() << "\n";
			if ((void*)store.GetStoreHostPointer() == nullptr)
				out << "Store Host Ptr: " << "nullptr" << "\n";
			else
				out << "Store Host Ptr: " << (void*)store.GetStoreHostPointer() << "\n";

			out << "Number of blocks: " << store.GetMemoryBlocksList()->size() << "\n";
			out << "\n";

			//For each block
			std::list<EngineAPI::Graphics::DeviceMemoryBlock>* blocks = store.GetMemoryBlocksList();
			std::list<EngineAPI::Graphics::DeviceMemoryBlock>::iterator it = blocks->begin();
			int blockIDX = 0;
			while (it != blocks->end())
			{
				EngineAPI::Graphics::DeviceMemoryBlock& block = *it;
				out << "	[Block: " << blockIDX << "] " << (void*)&block << "\n";
				out << "		Is Free: " << block.IsBlockFree() << "\n";
				out << "		Is Mappable: " << block.IsBlockMappable() << "\n";
				out << "		Is Mapped: " << block.IsBlockCurrentlyMapped() << "\n";
				out << "		Resource Size: " << block.GetResourceSize() << "\n";
				out << "		Resource Alignment: " << block.GetResourceAlignment() << "\n";

				if (block.GetResource() != nullptr)
				{
					out << "		Resource Address: " << (void*)block.GetResource() << "\n";
					out << "		Resource Type: " << block.GetResource()->GetResourceType() << "\n";
					if (!block.GetResource()->GetResourceDebugName().empty())
						out << "		Resource Debug Name: " << block.GetResource()->GetResourceDebugName().c_str() << "\n";
					else
						out << "		Resource Debug Name: " << "NULL" << "\n";
				}
				else
				{
					out << "		Resource Address: " << "nullptr" << "\n";
					out << "		Resource Type: " << "NULL" << "\n";
					out << "		Resource Debug Name: " << "NULL" << "\n";
				}

				out << "		Block Size: " << block.GetBlockSizeBytes() << "\n";
				out << "		Block Offset: " << block.GetBlockOffsetInStoreBytes() << "\n";
				out << "		Block Aligned Offset: " << block.GetBlockAlignedOffsetInStoreBytes() << "\n";
				
				if (block.GetBlockHostMemoryPointer()!= nullptr)
					out << "		Block Host Pointer: " << (void*)block.GetBlockHostMemoryPointer() << "\n";
				else
					out << "		Block Host Pointer: " << "nullptr" << "\n";


				//Next block
				out << "\n";
				++it;
				++blockIDX;
			}

			out << "**************************************************************" << "\n";
			out << "\n";
		}

		//Done
		out.close();
	}
}

EngineAPI::Graphics::DeviceMemoryStore* VulkanDeviceMemoryAllocator::CreateNewMemoryStore(EngineAPI::Graphics::RenderDevice* renderDevice,
	VkDeviceSize deviceMemorySizeInBytesToAlloc, uint32_t memoryTypeIndex, bool isPublicStore)
{
	//Fail if we have reached the maximum allowed stores
	if (deviceMemoryStoresCount >= ENGINE_CONFIG_VULKAN_API_MAX_NUMBER_OF_MEMORY_STORES)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryAllocator::CreateNewMemoryStore() Error - Too many active stores!\n");
		return nullptr;
	}

	VkDevice logicalDevice = renderDevice->GetVKLogicalDevice();
	VkPhysicalDeviceMemoryProperties memProperties = renderDevice->GetVKPhysicalDeviceMemoryProperties();

	//Init store
	if (!deviceMemoryStoresArray[deviceMemoryStoresCount].InitVKDeviceMemoryStore((DeviceMemoryAllocator*)this, &logicalDevice, deviceMemorySizeInBytesToAlloc, &memProperties, memoryTypeIndex, isPublicStore))
	{
		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryAllocator::CreateNewMemoryStore(): Could not init memory store!\n");
		return nullptr;
	}

	//New store, increment counter
	deviceMemoryStoresCount++;

	//Return pointer to this memory store
	return &(deviceMemoryStoresArray[deviceMemoryStoresCount - 1]);
}

SuballocationResult VulkanDeviceMemoryAllocator::AllocateResourceToStore(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Graphics::DeviceMemoryStore* store,
	const VkMemoryRequirements& resourceMemoryRequriments,
	EngineAPI::Rendering::Resource* resource)
{
	//Validate if the store can support this resource
	bool storeDoesSupportResource = store->DoesStoreSupportResource(resourceMemoryRequriments);
	if (!storeDoesSupportResource)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryAllocator::AllocateResourceToStore() - Store doesnt support this resource\n");
		return ALLOCATION_RESULT_STORE_MEMORY_TYPE_DOESNT_SUPPORT_RESOURCE_MEMORY_PROPERTIES;
	}

	//Allocate in to the given store
	SuballocationResult result = store->Private_Suballoc(resource, resourceMemoryRequriments.size, resourceMemoryRequriments.alignment, resourceMemoryRequriments.size);
	if (result != ALLOCATION_RESULT_SUCCESS)
	{
		if (result == ALLOCATION_RESULT_OUT_OF_MEMORY_FOR_STORE)
		{
			//Warning
			return result;
		}

		//Error
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryAllocator::AllocateResourceToStore() - Error: Could not alloc in to passed store\n");
		return result;
	}

	return result;
}

SuballocationResult VulkanDeviceMemoryAllocator::AllocateResourceAuto(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Rendering::Resource* resource)
{
	//Success when allocing memory?
	SuballocationResult success;

	//Alloc based on resource type
	RenderingResourceType resourceType = resource->GetResourceType();
	switch (resourceType)
	{
		case RENDERING_RESOURCE_TYPE_TEXTURE:
		{
			EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanDeviceMemoryAllocator::AllocateResourceAuto(): Allocating Texture\n");
			
			success = ALLOCATION_RESULT_SUCCESS;
			break;
		}
		case RENDERING_RESOURCR_TYPE_DEPTH_TEXTURE:
		{
			EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanDeviceMemoryAllocator::AllocateResourceAuto(): Allocating Depth Texture\n");
			
			//DepthTexture inherits from Texture which inherits from Resource. Thus, cast
			//the resource pointer to depth texture (we can then get its memory
			//requirements, texture handle, etc etc etc)
			//
			//Due to Resource::GetType(), we know the type so an implicit (down) cast will done nicely
			EngineAPI::Rendering::DepthTexture* depthResource = static_cast<EngineAPI::Rendering::DepthTexture*>(resource);
			
			//Vulkan memory requirements for this texture -> Helps us find the correct store
			//for this resource. 
			VkMemoryRequirements textureMemoryRequirments = depthResource->GetVKImageMemoryRequirments();
			
			//Device memory properties. Used with VkMemoryRequirments to work out what memory type
			//we want to use when allocing this depth texture
			VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = renderingDevice->GetVKPhysicalDeviceMemoryProperties();

			//Memory properties for this resource type
			VkMemoryPropertyFlags optimalFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			VkMemoryPropertyFlags fallbackFlags = 0;
			if (depthResource->IsTextureDynamic())
			{
				optimalFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
				fallbackFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
			}

			//Allocate on the device
			success = AllocTextureResourceAuto(renderingDevice, resource, resourceType, 
				optimalFlags, fallbackFlags,
				textureMemoryRequirments, physicalDeviceMemoryProperties);
			break;
		}
		case RENDERING_RESOURCE_TYPE_RENDER_TARGET_TEXTURE:
		{
			EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanDeviceMemoryAllocator::AllocateResourceAuto(): Allocating Render Target\n");
			
			success = ALLOCATION_RESULT_SUCCESS;
			break;
		}
		case RENDERING_RESOURCE_TYPE_BUFFER:
		{
			EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanDeviceMemoryAllocator::AllocateResourceAuto(): Allocating Buffer\n");
			
			success = ALLOCATION_RESULT_SUCCESS;
			break;
		}
		default:
		{	
			//Error
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryAllocator::AllocateResourceAuto(): Error - Unknown resource type\n");
			
			success = ALLOCATION_RESULT_SUCCESS;
			break;
		}
	}

	//Done
	return success;
}

SuballocationResult VulkanDeviceMemoryAllocator::AllocTextureResourceAuto(EngineAPI::Graphics::RenderDevice* renderingDevice,
	EngineAPI::Rendering::Resource* resource,
	RenderingResourceType resourceType,
	VkMemoryPropertyFlags resourceMemoryPropertyOptimalFlags,
	VkMemoryPropertyFlags resourceMemoryPropertyFallbackFlags,
	const VkMemoryRequirements& resourceMemoryRequirments,
	const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties)
{
	//Cast from resource to Texture (base class for depth texture, Texture2D etc etc etc)
	EngineAPI::Rendering::Texture* texture = static_cast<EngineAPI::Rendering::Texture*>(resource);
	//VkImageUsageFlags textureUsage = texture->GetVKImageUsageFlags();

	//Get the memory store that we can use for this texture. If this returns
	//NULL, no store could be found. As a result, we will have to create a brand
	//new store
	//
	//TODO: Ensure that SearchExistingPublicMemoryStoresToUseForResource() takes in to account
	//memory size + alignment when deciding if a store can be used. 
	EngineAPI::Graphics::DeviceMemoryStore* memStore = nullptr;
	memStore = SearchExistingPublicMemoryStoresToUseForResource(resourceMemoryPropertyOptimalFlags, resourceMemoryPropertyFallbackFlags,
			resourceMemoryRequirments, physicalDeviceMemoryProperties);
	if (memStore)
	{
		//Sub alloc in to this store.
		SuballocationResult result = memStore->Private_Suballoc(resource, resourceMemoryRequirments.size, resourceMemoryRequirments.alignment, resourceMemoryRequirments.size);
		if (result != ALLOCATION_RESULT_SUCCESS)
		{
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryAllocator::AllocTextureResourceAuto(): Could not alloc texture\n");
			return result;
		}
	}
	else
	{
		//Memory type index for this resource && new store
		uint32_t memoryTypeIDX = 0;
		if (!EngineAPI::Statics::VulkanStatics::FindMemoryTypeForProperties(resourceMemoryRequirments.memoryTypeBits,
			resourceMemoryPropertyOptimalFlags, &physicalDeviceMemoryProperties, &memoryTypeIDX))
		{
			//Use fallback properties
			if (!EngineAPI::Statics::VulkanStatics::FindMemoryTypeForProperties(resourceMemoryRequirments.memoryTypeBits,
				resourceMemoryPropertyFallbackFlags, &physicalDeviceMemoryProperties, &memoryTypeIDX))
			{
				//Error
				EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryAllocator::AllocTextureResourceAuto() - Error: Could not find any suitable memory type for this resource\n");
				return ALLOCATION_RESULT_COULD_NOT_CREATE_NEW_STORE;
			}
		}

		//Host local store or GPU store?
		bool isGPUOnlyStore = texture->IsTextureDynamic();

		//Size of allocation for the store
		VkDeviceSize storeSizeBytes = isGPUOnlyStore ? 
			MEB_TO_BYTES(ENGINE_CONFIG_VULKAN_API_DEVICE_LOCAL_STORE_SIZE_MB) : MEB_TO_BYTES(ENGINE_CONFIG_VULKAN_API_DEVICE_LOCAL_STORE_SIZE_MB);

		//Ensure we can fit this resource in the store - not an error, just go ahead and allow a larger store to fit this resource
		if (resourceMemoryRequirments.size > storeSizeBytes)
			storeSizeBytes = resourceMemoryRequirments.size;

		//Create new store for this memory
		EngineAPI::Graphics::DeviceMemoryStore* newStore = CreateNewMemoryStore(renderingDevice, storeSizeBytes, memoryTypeIDX, true);
		if (!newStore)
		{
			//Error
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryAllocator::AllocTextureResourceAuto(): Error - Could not create a new store for this resource\n");
			return ALLOCATION_RESULT_COULD_NOT_CREATE_NEW_STORE;
		}

		//Suballoc a block for this resource out of this store. 
		SuballocationResult result = newStore->Private_Suballoc(resource, resourceMemoryRequirments.size, resourceMemoryRequirments.alignment, resourceMemoryRequirments.size);
		if (result != ALLOCATION_RESULT_SUCCESS)
		{
			//Error
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryAllocator::AllocTextureResourceAuto(): Error - Could not suballocate a block for this resource\n");
			return result;
		}
	}

	//Done
	return ALLOCATION_RESULT_SUCCESS;
}


EngineAPI::Graphics::DeviceMemoryStore* VulkanDeviceMemoryAllocator::SearchExistingPublicMemoryStoresToUseForResource(
	VkMemoryPropertyFlags resourceMemoryPropertyOptimalFlags,
	VkMemoryPropertyFlags resourceMemoryPropertyFallbackFlags,
	const VkMemoryRequirements& resourceMemoryRequirments,
	const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties)
{
	//
	//TODO: Check if store can fit this resource before returning. If not, return OUT_OF_MEMORY error
	//

	//Memory type index we want for this resource?
	uint32_t memoryTypeIndexForResource = 0;
	bool didFind = EngineAPI::Statics::VulkanStatics::FindMemoryTypeForProperties(resourceMemoryRequirments.memoryTypeBits,
		resourceMemoryPropertyOptimalFlags, &physicalDeviceMemoryProperties, &memoryTypeIndexForResource);

	if (!didFind)
	{
		//Fallback - TODO: Make better. 
		bool didFindFallback = EngineAPI::Statics::VulkanStatics::FindMemoryTypeForProperties(resourceMemoryRequirments.memoryTypeBits,
			resourceMemoryPropertyFallbackFlags, &physicalDeviceMemoryProperties, &memoryTypeIndexForResource);

		if (!didFindFallback)
		{
			//Error
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanDeviceMemoryAllocator::SearchExistingPublicMemoryStoresToUseForResource()- Error: Could not find any memory types suitable for this resource\n");
			return false;
		}
	}

	//Does a store already exist for this resource with enough space left.
	for (int i = 0; i < deviceMemoryStoresCount; i++)
	{
		//Check if active
		if (!deviceMemoryStoresArray[i].IsMemoryStoreActive())
			break;

		//Only check for public stores
		bool storePublicFlag = deviceMemoryStoresArray[i].IsPublicMemoryStore();
		if (storePublicFlag)
		{
			//Yes. Is the stores memory type index the same as the one required by the
			//resource?
			uint32_t storeTypeIDX = deviceMemoryStoresArray[i].GetVKMemoryTypeIndex();
			bool isStoreCompatable = (storeTypeIDX == memoryTypeIndexForResource);
			if (isStoreCompatable)
			{
				//Yes! Is the stores max size > required bytes?
				VkDeviceSize storeSizeBytes = deviceMemoryStoresArray[i].GetMemoryStoreSizeBytes();
				if (storeSizeBytes >= resourceMemoryRequirments.size) //TODO: Alignment?
				{
					//Yes! Does this store have enough space for us to alloc 
					//this resource in to? TODO: Take in to account alignment
					EngineAPI::Graphics::DeviceMemoryBlock* lastBlock = deviceMemoryStoresArray[i].GetLastMemoryBlock();
					
					//Calculate the offset that would use for this resource + check if the
					//full resource would still fit within this store
					VkDeviceSize storeSizeRemaining = deviceMemoryStoresArray[i].GetMemoryStoreSizeBytes();
					if (lastBlock) //Could be nullptr if the store has no blocks
					{
						VkDeviceSize lastBlockOffset = lastBlock->GetBlockOffsetInStoreBytes();
						VkDeviceSize lastBlockSize = lastBlock->GetBlockSizeBytes();
						VkDeviceSize potentialOffsetForThisResource = lastBlockOffset + lastBlockSize;
						
						//Make offset aligned
						VkDeviceSize potentialOffsetForThisResourceAligned = EngineAPI::Statics::VulkanStatics::CalculateAlignedMemoryOffsetShiftRight(potentialOffsetForThisResource, resourceMemoryRequirments.alignment);
					
						//Using the aligned offset for this resource, calculate the remaining amount
						//of space this store has
						storeSizeRemaining -= potentialOffsetForThisResourceAligned;
					}

					//Enough space to use this store? 
					bool doesStoreHaveEnoughSpace = storeSizeRemaining >= resourceMemoryRequirments.size;
					if (doesStoreHaveEnoughSpace)
					{
						//Can suballoc here. Return it (we will later check if we can reuse
						//an existing block. If not, we will just create a new one)
						return &(deviceMemoryStoresArray[i]);
					}
				}
			}
		}
	}

	return nullptr;
}