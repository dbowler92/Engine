//VulkanShader.h
//Created 26/05/17
//Created By Daniel Bowler
//
//Represents a shader (Vertex, frag, tess, geometry, compute) module. In Vulkan, we can precompile
//shaders in to SPIR-V format (using glslangValidator). We can also compile at runtime (GLSL only???)
//if we prefer (this is useful as we can change the shader at runtime & recompile the shader. Thus, we
//can see changes to the program without needing to a) recompile the shader offline and b) restart
//the program)

#pragma once

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Device used to create this...
#include "../../../Graphics/Device/RenderDevice.h"

//Vulkan header
#include <vulkan/vulkan.h>

//Shader stage -> values match the Vulkan enum VkShaderStageFlagBits
enum ShaderStage
{
	SHADER_STAGE_VERTEX_SHADER = 0x00000001,
	SHADER_STAGE_TESSELLATION_CONTROL_SHADER = 0x00000002,
	SHADER_STAGE_TESSELLATION_EVALUATION_SHADER = 0x00000004,
	SHADER_STAGE_GEOMETRY_SHADER = 0x00000008,
	SHADER_STAGE_FRAGMENT_SHADER = 0x00000010,
	SHADER_STAGE_COMPUTE_SHADER = 0x00000020
};

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class VulkanShader
			{
			public:
				VulkanShader() {};
				virtual ~VulkanShader() = 0 {};

				//Shutsdown the shader
				void Shutdown();

				//Inits the shader from file. 
				bool InitVKShader(EngineAPI::Graphics::RenderDevice* renderingDevice,
					const char* shaderFile, ShaderStage shaderStage, const char* shaderEntryPointNameString, bool isPrecompiledSPIR);

			public:
				//Recompiles shader at runtime (If not a precompiled shader)
				bool RecompileShader();

			public:
				//Fills the VkPipelineShaderStageCreateInfo structure -> The structure is filled and used when creating
				//a VK pipeline object
				void FillVKPipelineShaderStageDescriptionStruct(VkPipelineShaderStageCreateInfo* createInfoOut);

			protected:
				//Info on this shader.
				std::string shaderFile;
				ShaderStage stage;
				bool isPrecompiledShader;
				std::string shaderEntryPointName; //eg: main

				//Vk assigned shader module handle
				VkShaderModule vkShaderModuleHandle = VK_NULL_HANDLE;

				//Cached device used to create the shader
				VkDevice cachedVKLogicalDevice = VK_NULL_HANDLE;

			private:
				//Converts from GLSL to SPIR-V format. Called if the shader is not precompiled
				//during init
				bool ConvertShaderFromGLSLToSPIRVFormat(const char* glslCode, uint32_t glslCodeSize, std::vector<char>* spirvCodeOut);

			private:
				//Simple function that reads a file
				bool ReadFile(const char* filename, std::vector<char>* fileOut);


			};
		};
	};
};