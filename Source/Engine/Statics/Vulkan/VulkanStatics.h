//VulkanStatics.h
//Created 27/04/17
//Created By Daniel Bowler
//
//A set of static helper functions && wrappers for Vulkan

#pragma once

//Globals 
#include "../../Main/EngineStd.h"

//Vulkan
#include <vulkan\vulkan.h>

//Forward declarations
namespace EngineAPI
{
	namespace Graphics
	{
		class GraphicsPipelineCache;
	};
};

namespace EngineAPI
{
	namespace Statics
	{
		class VulkanStatics
		{
			//Make staitcs only class (hide constructor, destructor etc)
			MAKE_STATICS_CLASS_ONLY(VulkanStatics)

		public:
			//Command buffers:
			//
			//Reset the command buffer. Note: The pool this command buffer comes from must
			//support individual buffer resets. 
			static bool CommandBufferReset(VkCommandBuffer* cmdBuffer, bool shouldReleaseMemoryToPool = false);

			//Begin / end VK command recording in to the cmdBuffer. Note: CommandBufferBeginRecordingDefault() will 
			//generate a default VkCommandBufferBeginInfo struct for us. 
			static bool CommandBufferBeginRecording(VkCommandBuffer* cmdBuffer, VkCommandBufferBeginInfo* cmdBufferBeginInfo);
			static bool CommandBufferBeginRecordingDefault(VkCommandBuffer* cmdBuffer);
			static bool CommandBufferEndRecording(VkCommandBuffer* cmdBuffer);

			//Memory
			//
			//Finds the memoryTypeIndex for a given memoryTypeBits (Obtained as a result of
			//a call to VkGet*MemoryRequirments) && properties (Eg: Device local vs CPU accessable)
			static bool FindMemoryTypeForProperties(uint32_t memoryTypeBits, VkMemoryPropertyFlags properties,
				const VkPhysicalDeviceMemoryProperties* physicalDeviceMemoryProperties,
				uint32_t* memTypeIndexOut);

			//Shifts an offset to the right such that it is memory aligned
			static VkDeviceSize CalculateAlignedMemoryOffsetShiftRight(VkDeviceSize memoryOffset, VkDeviceSize memoryAlignmentRequirment);

			//Graphics Pipeline
			//
			//Merges two Pipeline caches together. 
			static bool MergeGraphicsPipelineCaches(VkDevice* device,
				EngineAPI::Graphics::GraphicsPipelineCache* destinationPCO,
				EngineAPI::Graphics::GraphicsPipelineCache* sourcePipelineCachesArray, uint32_t sourcePipelinesCount);

			//Images
			//
			//Creates a VkImageView
			static bool CreateVKTextureView(VkDevice* device, VkImageViewCreateInfo* viewCreateInfo, VkImageView* imageViewHandleOut);
			static void DestoryVKTextureView(VkDevice* device, VkImageView* imageView);

			//Buffers
			//
			//Creates a VkBufferView
			static bool CreateVKBufferView(VkDevice* device, VkBufferViewCreateInfo* viewCreateInfo, VkBufferView* bufferViewHandleOut);
			static void DestroyVKBufferView(VkDevice* device, VkBufferView* bufferView);
		};
	};
};

//
//Seperate vulkan commands from general statics.
//

namespace EngineAPI
{
	namespace Statics
	{
		class VulkanCommands
		{
			//Make staitcs only class (hide constructor, destructor etc)
			MAKE_STATICS_CLASS_ONLY(VulkanCommands)

		public:
			//Writes the commands needed to set the image layout in to a command buffer
			/*
			static bool VKCMD_SetImageLayout(const VkImage& image,
				VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkAccessFlagBits srcAccessMask,
				const VkCommandBuffer& commandBuffer);
			*/

			static bool VKCMD_SetImageLayout(const VkCommandBuffer& commandBuffer, const VkImage& image,
				VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout);
		};
	};
};