#include "SandboxApplication.h"

#include "../App Config/AppConfig.h"

//Memory allocator
#include "../../Engine/Graphics/DeviceMemoryAllocator/DeviceMemoryAllocator.h"

//Statics 
#include "../../Engine/Statics/Vulkan/VulkanStatics.h"

SandboxApplication::SandboxApplication()
{}

ECHAR* SandboxApplication::GetGameTitle()
{
	return "Sandbox";
/*
#ifdef ENGINE_CONFIG_GRAPHICS_API_VULKAN
	return "Sandbox (Vulkan)"; 
#endif
*/
};

bool SandboxApplication::InitApplication()
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("SandboxApplication::InitApplication()\n");

	//Set clear info
	graphicsSubsystem->SetSwapchainClearValues(UNorm32Colour(0.0f, 0.0f, 1.0f, 1.0f), 1.0f, 0);

	//Device -> Used when creating rendering resources
	EngineAPI::Graphics::RenderDevice* device = EngineAPI::Graphics::GraphicsManager::GetInstance()->GetRenderingDevice();

	//Test Vertex buffer object:
	struct VertexWithColour
	{
		float x, y, z, w; // Vertex Position
		float r, g, b, a; // Colour format Red, Green, Blue, Alpha
	};

	VertexWithColour triangleData[] =
	{
		{ 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
		{ 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
		{ -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
	};

	//Streams
	VertexStreamDescription streams[2];
	streams[0].BufferBinding = 0;
	streams[0].Format = VK_FORMAT_R32G32B32A32_SFLOAT;
	streams[0].ShaderBinding = 0;
	streams[0].Offset = 0;

	streams[1].BufferBinding = 0;
	streams[1].Format = VK_FORMAT_R32G32B32A32_SFLOAT;
	streams[1].ShaderBinding = 1;
	streams[1].Offset = sizeof(float) * 4;

	//VB Description
	VertexBufferLayout vbLayout = {};
	vbLayout.Usage = VERTEX_BUFFER_USAGE_PER_VERTEX_DATA;
	vbLayout.BufferBinding = 0;
	vbLayout.VertexStride = sizeof(VertexWithColour);
	vbLayout.VertexStreamsCount = 2;
	vbLayout.VertexStreams = streams;

	bool isDynamicVB = true; //TEMP: For VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT memory rather than GPU only. 
	
	//Init the VB
	vb.SetResourceDebugName("Test Vertex Buffer");
	assert(vb.InitVKVertexBuffer(device, sizeof(triangleData), isDynamicVB));

	VkPhysicalDeviceMemoryProperties p = device->GetVKPhysicalDeviceMemoryProperties();

	//TEMP: Custom store for this resource.
	uint32_t memoryIndex = 0;
	assert(EngineAPI::Statics::VulkanStatics::FindMemoryTypeForProperties(vb.GetResourceVKMemoryRequirments().memoryTypeBits, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		&device->GetVKPhysicalDeviceMemoryProperties(), &memoryIndex));

	EngineAPI::Graphics::DeviceMemoryStore* customStoreForVB = nullptr;
	customStoreForVB = device->GetDeviceMemoryAllocator()->CreateNewMemoryStore(device,
			vb.GetResourceVKMemoryRequirments().size,
			memoryIndex,
			false);
	
	//Alloc VB
	assert(vb.AllocAndBindVKVertexBuffer(device, &vbLayout, &triangleData[0], customStoreForVB));

	//TEMP
	//device->GetDeviceMemoryAllocator()->Debug_LongDump("Dumps/PostVB_LabPC_DynamicVB_OwnStore");

	//
	//Shader
	//
	//assert(testShaderVS.InitVKShader(device, SHADER_ASSETS_FOLDER"TestShaders/Draw.vert", SHADER_STAGE_VERTEX_SHADER, false));
	//assert(testShaderFS.InitVKShader(device, SHADER_ASSETS_FOLDER"TestShaders/Draw.frag", SHADER_STAGE_FRAGMENT_SHADER, false));

	assert(testShaderSPIRVVS.InitVKShader(device, SHADER_ASSETS_FOLDER"TestShaders/Draw-vert.spv", SHADER_STAGE_VERTEX_SHADER, true));
	assert(testShaderSPIRVFS.InitVKShader(device, SHADER_ASSETS_FOLDER"TestShaders/Draw-frag.spv", SHADER_STAGE_FRAGMENT_SHADER, true));


	return true;
}

bool SandboxApplication::ShutdownApplication()
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("SandboxApplication::ShutdownApplication()\n");
	
	//Shutdown rendering data
	vb.Shutdown();

	//Shutdown shaders
	testShaderVS.Shutdown();
	testShaderFS.Shutdown();
	testShaderSPIRVVS.Shutdown();
	testShaderSPIRVFS.Shutdown();

	return true;
}

void SandboxApplication::UpdateScene(float dt)
{

}

void SandboxApplication::RenderScene()
{
	//Bind the swapchain for rendering
	assert(graphicsSubsystem->GetRenderingSwapchain()->BindAndClearSwapchainBuffers(graphicsSubsystem->GetRenderingDevice()));

	//
	//Render stuff here
	//

	//Present
	assert(graphicsSubsystem->GetRenderingSwapchain()->Present(graphicsSubsystem->GetRenderingDevice()));
}