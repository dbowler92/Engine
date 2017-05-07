//VulkanResoruce.h
//Created 07/05/17
//Created By Daniel Bowler
//
//Base class for the textures (eg: Texture2D, Texture2DArray etc) and buffers wrappers

#pragma once

enum VulkanResourceType
{
	VULKAN_RESOURCE_TYPE_TEXTURE,
	VULKAN_RESOURCE_TYPE_BUFFER
};

namespace EngineAPI
{
	namespace Rendering
	{
		namespace Platform
		{
			class VulkanResource
			{
			public:
				VulkanResource(VulkanResourceType type) { resourceType = type; };
				virtual ~VulkanResource() = 0 {};

				//Each resource will have a shutdown function to implement
				virtual void Shutdown() = 0;

			public:
				//Returns the resource type
				VulkanResourceType GetResourceType() { return resourceType; };

			protected:
				VulkanResourceType resourceType;
			};
		};
	};
};