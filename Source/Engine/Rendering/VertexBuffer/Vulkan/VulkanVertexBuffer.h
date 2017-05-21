//VulkanVertexBuffer.h
//Created 19/05/17
//Cretaed By Daniel Bowler
//
//VkBuffer which holds per vertex data (Designed to be an interleaved
//VB)

#pragma once

//Base class
#include "../../Buffer/Buffer.h"

//Does need to know about the device to create it
#include "../../../Graphics/Device/RenderDevice.h"

enum VertexBufferUsageFlag
{
	VERTEX_BUFFER_USAGE_PER_VERTEX_DATA,
	VERTEX_BUFFER_USAGE_PER_INSTANCE_DATA
};

//Describes one input in the vertex buffer. Eg: One for the
//local space positions, another from texcoords[0] and another
//for texcoords[1]
struct VertexStreamDescription
{
	uint32_t Binding;				 //Binding number from which this attribute takes its data			
	VkFormat Format;			     //Format of the data
	uint32_t ShaderBindingLocation;  //Shader binding location
	uint32_t Offset;				 //Offset in bytes of this attribute/stream relative to the start of an element
};

//Tells us the make up of the vertex buffer.
struct VertexBufferLayout
{
	VertexBufferUsageFlag Usage;				//VB Filled with per vertex or per instance data?
	VertexStreamDescription* VertexStreams;     //Describes the layout of the VB. Eg: Order of data, size etc
	uint32_t VertexStreamsCount;				//Number of pieces of input data
};

namespace EngineAPI
{
	namespace Rendering
	{
		namespace Platform
		{
			class VulkanVertexBuffer : public EngineAPI::Rendering::Buffer
			{
			public:
				VulkanVertexBuffer(RenderingResourceType type) : Buffer(type) {};
				virtual ~VulkanVertexBuffer() = 0 {};

				//Shutsdown the vertex buffer
				void Shutdown();

				//1) Inits the vertex buffer
				bool InitVKVertexBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice, 
					VkDeviceSize vertexBufferSizeBytes, VkDeviceSize vertexBufferStrideBytes, bool isDynamicVertexBuffer);

				//2) Allocates the vertex buffer on the device.
				bool AllocAndBindVKVertexBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
					VertexBufferLayout* vertexBufferLayout, void* vertexBufferData, 
					EngineAPI::Graphics::DeviceMemoryStore* optionalDeviceStore = nullptr);

			protected:
				//VK info
				VkVertexInputBindingDescription inputRate; //Per buffer info (TBC)
				std::vector<VkVertexInputAttributeDescription> inputAttributes; //Per stream info

			protected:
				//Cached info on the VB
				VkDeviceSize vertexBufferStride = 0;

			};
		};
	};
};