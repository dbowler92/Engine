#include "VulkanShader.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanShader::Shutdown()
{

}

void VulkanShader::InitVKShader(const char* shaderFile, ShaderStage shaderStage, bool isPrecompiledSPIR)
{
	//Cache info
	stage = shaderStage;
	isPrecompiledShader = isPrecompiledSPIR;

	//
}