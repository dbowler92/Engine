//VulkanProgram.h
//Created 30/05/17
//Created By Daniel Bowler
//
//Contains a list of Vulkan shaders modules. Linked together to form
//a complete shader program

#pragma once

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Device used to create this...
#include "../../../Graphics/Device/RenderDevice.h"

//Manages a set of shaders
#include "../../../Graphics/Shader/Shader.h"

//Vulkan header
#include <vulkan/vulkan.h>

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanProgram
			{
			public:
				VulkanProgram() {};
				virtual ~VulkanProgram() = 0 {};

				//Shutsdown the program and shaders (if true - this depends on who originally created
				//the shaders -> The program class or the programmer elsewhere)
				void Shutdown(bool doShutdownShaders = false);

				//Adds a vulkan shader module to the list
				bool CreateVKShaderModule(EngineAPI::Graphics::RenderDevice* renderingDevice,
					const char* shaderFile, ShaderStage shaderStage, const char* shaderEntryPointNameString, bool isPrecompiledSPIR);
				bool AddVKShaderModule(EngineAPI::Graphics::Shader& shader);

			public:
				//Returns the shaders & number of them. These can then be used to populate an array of the
				//VkPipelineShaderStageCreateInfo when creating the pipeline object. 
				EngineAPI::Graphics::Shader* GetAttachedVKShaders() { return shadersArray.data(); };
				uint32_t GetAttachedVKShadersCount() { return (uint32_t)shadersArray.size(); };

			protected:
				//Array of shaders - TODO: Statically sized array?
				std::vector<EngineAPI::Graphics::Shader> shadersArray;
			};
		};
	};
};