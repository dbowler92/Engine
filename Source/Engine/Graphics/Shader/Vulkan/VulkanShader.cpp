#include "VulkanShader.h"

//Reading GLSL -to- SPIR-V format. 
#include "SPIRV/GlslangToSpv.h"

#include <fstream> //File reading -> Currently, we do not have any custom fle IO system. :(

using namespace EngineAPI::Graphics::Platform;

void VulkanShader::Shutdown()
{
	if (vkShaderModuleHandle)
	{
		vkDestroyShaderModule(cachedVKLogicalDevice, vkShaderModuleHandle, nullptr);
		vkShaderModuleHandle = VK_NULL_HANDLE;
	}
}

bool VulkanShader::InitVKShader(EngineAPI::Graphics::RenderDevice* renderingDevice,
	const char* shaderFile, ShaderStage shaderStage, const char* shaderEntryPointNameString, bool isPrecompiledSPIR)
{
	//Cache info
	shaderFile = shaderFile;
	stage = shaderStage;
	isPrecompiledShader = isPrecompiledSPIR;
	cachedVKLogicalDevice = renderingDevice->GetVKLogicalDevice();
	shaderEntryPointName = shaderEntryPointNameString;

	//Print info on what we are doing
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanShader::InitVKShader(): Loading shader: ");
	EngineAPI::Debug::DebugLog::PrintMessage(shaderFile);
	EngineAPI::Debug::DebugLog::PrintMessage("\n");

	std::vector<char> spirvCode;
	if (!isPrecompiledShader)
	{
		//if not already in SPIRV format, convert.
		//
		//Parse GLSL file as text...
		std::vector<char> glslCode;
		if (!ReadFile(shaderFile, &glslCode))
		{
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanShader::InitVKShader() Error: Failed to read GLSL file\n");
			return false;
		}

		//Convert to SPIR-V format using 3rd party libraries. 
		if (!ConvertShaderFromGLSLToSPIRVFormat(glslCode.data(), glslCode.size(), &spirvCode))
		{
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanShader::InitVKShader() Error: Could not convert from GLSL to SPIR-V format\n");
			return false;
		}

		//Cleanup GLSL parsed file as its not needed anymore. 
		glslCode.clear();
	}
	else
	{
		//Parse SPIRV file
		if (!ReadFile(shaderFile, &spirvCode))
		{
			EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanShader::InitVKShader() Error: Failed to read SPIR-V file\n");
			return false;
		}
	}

	//Aligned SPIR-V code
	std::vector<uint32_t> alignedSPIRVCode(spirvCode.size() / sizeof(uint32_t) + 1);
	memcpy(alignedSPIRVCode.data(), spirvCode.data(), spirvCode.size());

	//Once data is in SPIRV format, create the shader module
	VkShaderModuleCreateInfo shaderCreateInfo = {};
	shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderCreateInfo.pNext = nullptr;
	shaderCreateInfo.flags = 0;
	shaderCreateInfo.codeSize = spirvCode.size();
	shaderCreateInfo.pCode = alignedSPIRVCode.data();

	VkResult result = vkCreateShaderModule(cachedVKLogicalDevice, &shaderCreateInfo, nullptr, &vkShaderModuleHandle);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanShader::InitVKShader() Error: Failed to create shader module\n");
		return false;
	}
	
	//Done
	return true;
}

bool VulkanShader::RecompileShader()
{
	//Can't recompile something already in SPIR-V format. Perhaps allow a complete
	//reload? 
	if (isPrecompiledShader)
	{
		EngineAPI::Debug::DebugLog::PrintWarningMessage("VulkanShader::RecompileShader(). Can not recompile this shader -> It was precompiled\n");
		return false;
	}

	//TODO
	EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanShader::RecompileShader() Error: Not yet implemented");
	return false;

	//Done
	return true;
}

void VulkanShader::FillVKPipelineShaderStageDescriptionStruct(VkPipelineShaderStageCreateInfo* createInfoOut)
{
	//Pipeline description for this shader module
	*createInfoOut = {};
	createInfoOut->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfoOut->pNext = nullptr;
	createInfoOut->flags = 0;
	createInfoOut->stage = (VkShaderStageFlagBits)stage;
	createInfoOut->module = vkShaderModuleHandle;
	createInfoOut->pName = shaderEntryPointName.c_str(); //Entry point for the shader. 
	createInfoOut->pSpecializationInfo = nullptr; //TODO?
}

bool VulkanShader::ConvertShaderFromGLSLToSPIRVFormat(const char* glslCode, uint32_t glslCodeSize, std::vector<char>* spirvCodeOut)
{
	//TODO
	EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanShader::ConvertShaderFromGLSLToSPIRVFormat() Error: Not yet implemented");
	return false;

	//Done
	return true;
}

bool VulkanShader::ReadFile(const char* filename, std::vector<char>* fileOut)
{
	//Open file
	std::ifstream file(filename, std::ios::ate | std::ios::binary); //Start read at the end -> We can then easily calculate the file size
	if (!file.is_open())
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanShader::ReadGLSLFile() Error: Could not open file\n");
		return false;
	}

	//Is open, copy data. First, resize output vector
	size_t fileSize = (size_t)file.tellg();
	fileOut->resize(fileSize);
	
	//Seek to beginning of the file before copy
	file.seekg(0);

	//Copy data
	file.read(fileOut->data(), fileSize);

	//Close file and return
	file.close();

	//Done
	return true;
}