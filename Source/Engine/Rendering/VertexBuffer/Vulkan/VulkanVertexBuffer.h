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

				//Inits the vertex buffer
				bool InitVKVertexBuffer(EngineAPI::Graphics::RenderDevice* renderingDevice, 
					VkDeviceSize vertexBufferSizeBytes, void* vertexBufferData,
					bool isDynamicVertexBuffer);
			};
		};
	};
};