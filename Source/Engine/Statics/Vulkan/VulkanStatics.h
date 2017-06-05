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

			//Sync
			//
			//Creates a fence, semaphore and events
			static bool CreateVKFence(VkDevice* device, bool isInitedInSignaledState, VkFence* fenceOut);
			static bool CreateVKSemaphore(VkDevice* device, VkSemaphore* semaphoreOut);
			static bool CreateVKEvent(VkDevice* device, VkEvent* eventOut);

		};
	};
};

//
//Seperate vulkan commands from general statics.
//
//VKCMD_* => Simple Wrapper around Vulkan commands. 
//CMD_* => Abstraction
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
			static bool CMD_SetImageLayout(const VkImage& image,
				VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkAccessFlagBits srcAccessMask,
				const VkCommandBuffer& commandBuffer);
			*/
			static bool CMD_SetImageLayout(const VkCommandBuffer& commandBuffer, const VkImage& image,
				VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout);

			//Changes subpass
			static void VKCMD_NextSubpass(const VkCommandBuffer& commandBuffer, VkSubpassContents contents);

			//Binds a pipeline
			static void VKCMD_BindGraphicsPipeline(const VkCommandBuffer& commandBuffer, VkPipeline pipelineHandle);
			static void VKCMD_BindComputePipeline(const VkCommandBuffer& commandBuffer, VkPipeline pipelineHandle);

			//Binds vertex buffer(s). 
			static void VKCMD_BindVertexBuffers(const VkCommandBuffer& commandBuffer,
				uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* buffersArray, const VkDeviceSize* offsetsArray);

			//Binds index buffer
			static void VKCMD_BindIndexBuffer(const VkCommandBuffer& commandBuffer,
				VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType);

			//Dynamically change viewport state
			static void VKCMD_DynamicallySetViewports(const VkCommandBuffer& commandBuffer,
				uint32_t firstViewport, uint32_t viewportCount, const VkViewport* viewportsArray);

			//Dynamically change the scissoring state
			static void VKCMD_DynamicallySetScissors(const VkCommandBuffer& commandBuffer,
				uint32_t firstScissor, uint32_t scissorsCount, const VkRect2D* scissorsArray);

			//Drawing commands
			static void VKCMD_Draw(const VkCommandBuffer& commandBuffer, 
				uint32_t vertexCount, uint32_t instanceCount, 
				uint32_t firstVertex, uint32_t firstInstance);
			static void VKCMD_DrawIndexed(const VkCommandBuffer& commandBuffer,
				uint32_t indexCount, uint32_t instanceCount,
				uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);

			//Indirect drawing commands
			static void VKCMD_DrawIndirect(const VkCommandBuffer& commandBuffer,
				VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride);
			static void VKCMD_DrawIndexedIndirect(const VkCommandBuffer& commandBuffer,
				VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride);
		};
	};
};

//
//Temp: Quick way of filling out VkPipeline*StateCreateInfo struct. Will abstract this stuff
//later. 
//
const float DEFAULT_BLEND_CONSTANTS[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

namespace EngineAPI
{
	namespace Statics
	{
		class VulkanStates
		{
			//Statics only class - hides constructor etc
			MAKE_STATICS_CLASS_ONLY(VulkanStates)

		public:
			//Dynamic states
			static void GeneratePipelineDynamicStatesCreateStruct(VkPipelineDynamicStateCreateInfo* structOut,
				const VkDynamicState* dynamicStates, uint32_t dynamicStatesCount);

			//Vertex input state (Vertex buffer)
			static void GeneratePipelineVertexInputCreateStruct(VkPipelineVertexInputStateCreateInfo* structOut,
				const VkVertexInputBindingDescription* inputBindings, uint32_t inputBindingsCount, 
				const VkVertexInputAttributeDescription* inputAttributes, uint32_t inputAttributesCount);

			//Input assembly state (Topology etc)
			static void GeneratePipelineInputAssemblyCreateStruct(VkPipelineInputAssemblyStateCreateInfo* structOut, 
				VkPrimitiveTopology topology, VkBool32 primitiveRestartEnabled);

			//Raster state
			static void GeneratePipelineRasterStateCreateStruct(VkPipelineRasterizationStateCreateInfo* structOut, 
				VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFaceMode, 
				VkBool32 depthClampEnabled, VkBool32 rasterizerDiscardEnabled = VK_FALSE, VkBool32 depthBiasEnabled = VK_FALSE,
				float depthBiasConstantFactor = 0.0f, float depthBiasClamp = 0.0f, float depthBiasSlopeFactor = 0.0f, float lineWidth = 1.0f);

			//Colour blending
			static void GeneratePipelineColourBlendCreateStruct(VkPipelineColorBlendStateCreateInfo* structOut,
				VkBool32 logicalOpEnabled, VkLogicOp logicalOp,
				const VkPipelineColorBlendAttachmentState* attachments, uint32_t attachmentsCount,
				const float blendConstants[4] = DEFAULT_BLEND_CONSTANTS);

			//Viewport(s) state && scissor states
			static void GeneratePipelineViewportStateCreateStruct(VkPipelineViewportStateCreateInfo* structOut, 
				VkViewport* viewports, uint32_t viewportsCount, 
				const VkRect2D* scissors, uint32_t scissorsCount);

			//Depth/Stencil state
			static void GeneratePipelineDepthStencilStateCreateStruct(VkPipelineDepthStencilStateCreateInfo* structOut,
				VkBool32 depthTestEnabled, VkBool32 depthWriteEnabled, VkBool32 stencilTestEnabled = VK_FALSE, VkBool32 depthBoundsTestEnabled = VK_FALSE,
				VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
				VkStencilOpState frontStencilOp = {}, VkStencilOpState backStencilOp = {},
				float minDepthBounds = 0.0f, float maxDepthBounds = 1.0f);

			//Multisample state
			static void GeneratePipelineMultisampleStateCreateStruct(VkPipelineMultisampleStateCreateInfo* structOut, 
				VkSampleCountFlagBits sampleCountFlag, VkBool32 sampleShadingEnabled = VK_FALSE, 
				float minSampleShading = 0.0f, const VkSampleMask* sampleMask = nullptr, 
				VkBool32 alphaToCoverageEnabled = VK_FALSE, VkBool32 alphaToOneEnabled = VK_FALSE);

			//TODO: Tessellation
		};
	};
};