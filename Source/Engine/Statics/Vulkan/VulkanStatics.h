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

namespace EngineAPI
{
	namespace Graphics
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

			//Images
			//
			//Creates a VkImageView
			static bool CreateVKTextureView(VkDevice* device, VkImageViewCreateInfo* viewCreateInfo, VkImageView* imageViewHandleOut);
		
		};
	}
}