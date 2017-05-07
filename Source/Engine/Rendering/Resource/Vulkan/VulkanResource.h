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
	namespace Rendering
	{
		namespace Platform
		{
			class VulkanResource
			{
			public:
				VulkanResource(RenderingResourceType type) { resourceType = type; };
				virtual ~VulkanResource() = 0 {};

				//Each resource will have a shutdown function to implement
				virtual void Shutdown() = 0;

			public:
				//Returns the resource type
				RenderingResourceType GetResourceType() { return resourceType; };

			protected:
				RenderingResourceType resourceType;
			};
		};
	};
};