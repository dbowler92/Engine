//VulkanRenderCommandBuffer.h
//Created 21/04/17
//Created by Daniel Bowler
//
//Represents the vulkan command buffer that will be sent to 
//the GPU for processing. 

#pragma once

//Base class
#include "../Common/CommonRenderCommandBuffer.h"

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanRenderCommandBuffer : public EngineAPI::Graphics::Interface::CommonRenderCommandBuffer
			{
			public:
				VulkanRenderCommandBuffer() {};
				virtual ~VulkanRenderCommandBuffer() = 0 {};

				//Override init and shutdown functions providing vulkan specific
				//implementation
				bool Init(EngineAPI::Graphics::RenderDevice* renderDevice, 
					EngineAPI::Graphics::RenderInstance* renderInstance);
				void Shutdown();

				//Captures vulkan rendering commands in to a reusable command
				//buffer
				bool BeginRecordingToCommandBuffer() override;
				bool EndRecordingToCommandBuffer() override;
			};
		};
	};
};