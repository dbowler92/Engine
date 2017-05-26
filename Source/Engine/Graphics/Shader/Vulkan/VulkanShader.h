//VulkanShader.h
//Created 26/05/17
//Created By Daniel Bowler
//
//Represents a shader (Vertex, frag, tess, geometry, compute). In Vulkan, we can precompile
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

enum ShaderStage
{
	SHADER_STAGE_VERTEX_SHADER,
	SHADER_STAGE_FRAGMENT_SHADER
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
					const char* shaderFile, ShaderStage shaderStage, bool isPrecompiledSPIR);

			public:
				//Recompiles shader at runtime (If not a precompiled shader)
				bool RecompileShader();

			protected:
				//Info on this shader.
				std::string shaderFile;
				ShaderStage stage;
				bool isPrecompiledShader;

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