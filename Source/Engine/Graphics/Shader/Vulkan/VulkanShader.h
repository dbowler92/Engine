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
				void InitVKShader(const char* shaderFile, ShaderStage shaderStage, bool isPrecompiledSPIR);

			protected:
				//Info on this shader.
				ShaderStage stage;
				bool isPrecompiledShader;
			};
		};
	};
};