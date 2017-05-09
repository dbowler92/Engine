//VulkanResoruce.h
//Created 07/05/17
//Created By Daniel Bowler
//
//Base class for the textures (eg: Texture2D, Texture2DArray etc) and buffers wrappers

#pragma once

enum RenderingResourceType
{
	RENDERING_RESOURCE_TYPE_TEXTURE,
	RENDERING_RESOURCR_TYPE_DEPTH_TEXTURE, 
	RENDERING_RESOURCE_TYPE_RENDER_TARGET_TEXTURE,
	RENDERING_RESOURCE_TYPE_BUFFER
};

namespace EngineAPI
{
	namespace Graphics
	{
		class DeviceMemoryBlock;
		namespace Platform
		{
			class VulkanDeviceMemoryBlock;
		};
	};
};


namespace EngineAPI
{
	namespace Rendering
	{
		namespace Platform
		{
			class VulkanResource
			{
				friend EngineAPI::Graphics::Platform::VulkanDeviceMemoryBlock;
			public:
				VulkanResource(RenderingResourceType type) { resourceType = type; };
				virtual ~VulkanResource() = 0 {};

				//Each resource will have a shutdown function to implement
				virtual void Shutdown() = 0;

			public:
				//Returns the resource type
				RenderingResourceType GetResourceType() { return resourceType; };

			protected:
				//Resource type -> Set by subclasses (constructor)
				RenderingResourceType resourceType;

				//Reference to the memory block for this resource -> This is set
				//by the private Private_SetDeviceMemoryBlockPointer() function when allocated 
				//by the block itself - Don't reset this pointer. 
				EngineAPI::Graphics::DeviceMemoryBlock* resourceMemoryBlock = nullptr;

			private:
				//Visible to DeviceMemoryBlock
				void Private_SetDeviceMemoryBlockPointer(EngineAPI::Graphics::DeviceMemoryBlock* block) { resourceMemoryBlock = block; };
			};
		};
	};
};