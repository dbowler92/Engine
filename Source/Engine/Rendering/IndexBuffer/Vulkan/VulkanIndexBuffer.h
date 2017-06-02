//VulkanIndexBuffer.h
//Created 02/06/17
//Created By Daniel Bowler
//
//Represents a vulkan buffer storing index data

#pragma once

//Base class
#include "../../Buffer/Buffer.h"

//Does need to know about the device to create it
#include "../../../Graphics/Device/RenderDevice.h"

enum IndexElementDataSize
{
	INDEX_ELEMENT_DATA_SIZE_UINT_16,
	INDEX_ELEMENT_DATA_SIZE_UINT_32
};

//Tells us the make up of the index buffer.
struct IndexBufferLayout
{
	IndexBufferLayout()
	{};

	IndexElementDataSize IndexStride; //Bytes between each index. Basically, are we using uint32 or uint16
};

namespace EngineAPI
{
	namespace Rendering
	{
		namespace Platform
		{
			class VulkanIndexBuffer : public EngineAPI::Rendering::Buffer
			{
			public:
				VulkanIndexBuffer(RenderingResourceType type) : Buffer(type) {};
				virtual ~VulkanIndexBuffer() = 0 {};

				//Shutdown the index buffer
				void Shutdown();

				//1) Init the index buffer
				bool InitVKIndexBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
					VkDeviceSize indexBufferSizeBytes, bool isDynamicIndexBuffer);

				//2) Allocates the index buffer on the device -> This includes writing
				//data to the buffer (staging) & binding the device memory
				bool AllocAndBindVKIndexBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice,
					IndexBufferLayout* indexBufferLayout, void* indexBufferData,
					EngineAPI::Graphics::DeviceMemoryStore* optionalDeviceStore = nullptr);

				//3) (Optional) Creates the buffer views - if the buffer is to be utilized by 
				//shaders, we may need to create some buffer views
				bool InitVKBufferViews(EngineAPI::Graphics::RenderDevice* renderingDevice);

			public:
				//Gets the index type of the IB
				VkIndexType GetVKIndexType();

				//Get element count
				uint32_t GetVKIndexBufferElementCount() { return indexBufferElementsCount; };

			protected:
				//Index buffer info
				IndexElementDataSize indexBufferStride;
				uint32_t indexBufferElementsCount = 0;
			};
		};
	};
};
