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
	uint32_t BufferBinding;			 //Binding number (buffer) from which this attribute takes its data			
	VkFormat Format;			     //Format of the data
	uint32_t ShaderBinding;		     //Shader binding location
	uint32_t Offset;				 //Offset in bytes of this attribute/stream relative to the start of an element
};

//Tells us the make up of the vertex buffer.
struct VertexBufferLayout
{
	VertexBufferLayout() 
	{ 
		VertexStreams = nullptr;  
		VertexStreamsCount = 0; 
	};

	VertexBufferUsageFlag Usage;				//VB Filled with per vertex or per instance data?
	uint32_t BufferBinding;						//Buffer binding point (Could have multiple bound buffers)
	VkDeviceSize VertexStride;					//Bytes between each individual element (vertex)
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
					VkDeviceSize vertexBufferSizeBytes, bool isDynamicVertexBuffer);

				//2) Allocates the vertex buffer on the device -> This includes writing
				//data to the buffer (staging) & binding the device memory
				bool AllocAndBindVKVertexBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
					VertexBufferLayout* vertexBufferLayout, void* vertexBufferData, 
					EngineAPI::Graphics::DeviceMemoryStore* optionalDeviceStore = nullptr);

				//3) (Optional) Creates the buffer views - if the buffer is to be utilised by 
				//shaders, we may need to create some buffer views
				bool InitVKBufferViews(EngineAPI::Graphics::RenderDevice* renderingDevice);

			public:
				VkVertexInputBindingDescription* GetInputBindingDescription() { return &bufferBindingInfo; };
				VkVertexInputAttributeDescription* GetInputAttributesDescriptions() { return inputAttributes.data(); };
				uint32_t GetInputAttributesDescriptionsCount() { return inputAttributes.size(); };

				//Gets the number of vertex elements in the buffer
				uint32_t GetVKVertexBufferElementsCount() { return vertexBufferElementsCount; };

			protected:
				//Vulkan info
				VkVertexInputBindingDescription bufferBindingInfo; //Per buffer info (Rate at which this buffers info will be injected for vertex input)
				std::vector<VkVertexInputAttributeDescription> inputAttributes; //Per stream info (Helps interpret the data & map from buffer to shader attributes)

			protected:
				//Vertex Buffer info
				VkDeviceSize vertexBufferStride = 0;
				uint32_t vertexBufferElementsCount = 0;
			};
		};
	};
};