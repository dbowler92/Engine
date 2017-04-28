//VulkanStatics.h
//Created 27/04/17
//Created By Daniel Bowler
//
//A set of static helper functions && wrappers for Vulkan

#pragma once

//Globals 
#include "../../../Main/EngineStd.h"

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
		};
	}
}
