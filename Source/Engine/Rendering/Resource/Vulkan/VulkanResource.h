//VulkanResoruce.h
//Created 07/05/17
//Created By Daniel Bowler
//
//Base class for the textures (eg: Texture2D, Texture2DArray etc) and buffers wrappers

#pragma once

//String
#include <string>

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Static helper
#include "../../../Statics/Vulkan/VulkanStatics.h"

//Can work out if a resource is a:
//	Texture if resource->GetType() < RENDERING_RESOURCE_TYPE_ENUM_BUFFERS_BEGIN
//	Buffer if resource->GetType() > RENDERING_RESOURCE_TYPE_ENUM_BUFFERS_BEGIN

enum RenderingResourceType
{
	RENDERING_RESOURCE_TYPE_ENUM_TEXTURES_BEGIN,  //*

	RENDERING_RESOURCE_TYPE_SAMPLER_2D,
	RENDERING_RESOURCR_TYPE_DEPTH_TEXTURE, 
	RENDERING_RESOURCE_TYPE_RENDER_TARGET_TEXTURE,

	RENDERING_RESOURCE_TYPE_ENUM_BUFFERS_BEGIN,  //*

	RENDERING_RESOURCE_TYPE_VERTEX_BUFFER,
	RENDERING_RESOURCE_TYPE_INDEX_BUFFER,
	RENDERING_RESOURCE_TYPE_UNIFORM_BUFFER,

	RENDERING_RESOURCE_TYPE_STAGING_BUFFER
};

//Resource usage -> This is TODO!
enum RenderingResourceUsage
{
	RENDERING_RESOURCE_USAGE_GPU_READ_WRITE_ONLY,	    //(Default)
	RENDERING_RESOURCE_USAGE_GPU_READ_ONLY,				//(Immutable)
	RENDERING_RESOURCE_USAGE_GPU_READ_CPU_WRITE,		//(Dynamic)
	RENDERING_RESOURCE_USAGE_GPU_READ_CPU_READ_WRITE	//(Staging)
};

namespace EngineAPI
{
	namespace Graphics
	{
		class RenderDevice;
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
				//Maps the resource for CPU read(?)- write
				void* MapResource();
				void UnmapResource();

			public:
				//Returns the resource type
				RenderingResourceType GetResourceType() { return resourceType; };

				//Returns the device memory block for this resource
				EngineAPI::Graphics::DeviceMemoryBlock* GetDeviceMemoryBlock() { return resourceMemoryBlock; };

			public:
				//Resource info getters
				VkMemoryRequirements GetResourceVKMemoryRequirments() { return vkResourceMemoryRequirments; };
				RenderingResourceUsage GetResourceUsage() { return resourceUsage; };

			public:
				//Debug name -> Used when dumping info (Eg: State of the allocator)
				void SetResourceDebugName(std::string name) { debugName = name; };
				std::string GetResourceDebugName() { return debugName; };

			protected:
				//Debug resource identifier (name)
				std::string debugName;

				//Resource type -> Set by subclasses (constructor)
				RenderingResourceType resourceType;

				//Reference to the memory block for this resource -> This is set
				//by the private Private_SetDeviceMemoryBlockPointer() function when allocated 
				//by the block itself - Don't reset this pointer. 
				EngineAPI::Graphics::DeviceMemoryBlock* resourceMemoryBlock = nullptr;

			protected:
				//Resource usage. 
				RenderingResourceUsage resourceUsage;

				//Vulkan memory requirements for the resource
				VkMemoryRequirements vkResourceMemoryRequirments;

			private:
				//Visible to DeviceMemoryBlock
				void Private_SetDeviceMemoryBlockPointer(EngineAPI::Graphics::DeviceMemoryBlock* block) { resourceMemoryBlock = block; };
			};
		};
	};
};