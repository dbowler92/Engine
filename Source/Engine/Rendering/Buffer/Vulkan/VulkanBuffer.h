//VulkanBuffer.h
//Created 18/05/17
//Created By Daniel Bowler
//
//Represents a Vulkan buffer resource

#pragma once

//Is a resource
#include "../../Resource/Resource.h"

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Device used to create this...
#include "../../../Graphics/Device/RenderDevice.h"

//Stores
#include "../../../Graphics/DeviceMemoryStore/DeviceMemoryStore.h"

//Vulkan header
#include <vulkan/vulkan.h>

namespace EngineAPI
{
	namespace Rendering
	{
		namespace Platform
		{
			class VulkanBuffer : public EngineAPI::Rendering::Resource
			{
			public:
				VulkanBuffer(RenderingResourceType type) : Resource(type) {};
				virtual ~VulkanBuffer() = 0 {};

				//Shutodwn the buffer object - called manually by the subclasses. 
				virtual void Shutdown() = 0;

			};
		};
	};
};