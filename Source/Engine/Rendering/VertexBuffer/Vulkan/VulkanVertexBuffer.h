//VulkanVertexBuffer.h
//Created 19/05/17
//Cretaed By Daniel Bowler
//
//VkBuffer which holds per vertex data

#pragma once

//Base class
#include "../../Buffer/Buffer.h"

//Does need to know about the device to create it
#include "../../../Graphics/Device/RenderDevice.h"

struct PerVertexLayoutDescription
{
	uint32_t BindingIndex;
	VkFormat Format;
	uint32_t Location;
	uint32_t Offset;
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

				//2) Allocates the vertex buffer.
				bool AllocAndBindVKVertexBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice, void* vertexBufferData);

				//3) Init vertex buffer views
				bool InitVKVertexBufferViews(EngineAPI::Graphics::RenderDevice* renderingDevice);
			};
		};
	};
};