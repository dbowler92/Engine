#include "VulkanProgram.h"

using namespace EngineAPI::Graphics::Platform;

void VulkanProgram::Shutdown(bool doShutdownShaders)
{
	if (doShutdownShaders)
	{
		for (int i = 0; i < shadersArray.size(); i++)
			shadersArray[i].Shutdown();
	}

	shadersArray.clear();
}

bool VulkanProgram::CreateVKShaderModule(EngineAPI::Graphics::RenderDevice* renderingDevice,
	const char* shaderFile, ShaderStage shaderStage, const char* shaderEntryPointNameString, bool isPrecompiledSPIR)
{
	//TODO: Validate

	//New shader
	shadersArray.resize(shadersArray.size() + 1);

	//Init
	if (!shadersArray[shadersArray.size() - 1].InitVKShader(renderingDevice, shaderFile, shaderStage, shaderEntryPointNameString, isPrecompiledSPIR))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanProgram::CreateVKShaderModuleAndAddToProgram() Error initing new shader module\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanProgram::AddVKShaderModule(EngineAPI::Graphics::Shader& shader)
{
	//New shader
	shadersArray.resize(shadersArray.size() + 1);

	//Copy - TODO: Override = operator -> Currently this is a shallow copy which is fine for now, 
	//but may change in the future! 
	shadersArray[shadersArray.size() - 1] = shader;

	//Done
	return true;
}