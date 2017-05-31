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
	//assert(testShaderVS.InitVKShader(device, SHADER_ASSETS_FOLDER"TestShaders/Draw.vert", SHADER_STAGE_VERTEX_SHADER, "main", false));
	//assert(testShaderFS.InitVKShader(device, SHADER_ASSETS_FOLDER"TestShaders/Draw.frag", SHADER_STAGE_FRAGMENT_SHADER, "main", false));

	//assert(testShaderSPIRVVS.InitVKShader(device, SHADER_ASSETS_FOLDER"TestShaders/Draw-vert.spv", SHADER_STAGE_VERTEX_SHADER, "main", true));
	//assert(testShaderSPIRVFS.InitVKShader(device, SHADER_ASSETS_FOLDER"TestShaders/Draw-frag.spv", SHADER_STAGE_FRAGMENT_SHADER, "main", true));

	assert(testProgramSPIR.CreateVKShaderModule(device, SHADER_ASSETS_FOLDER"TestShaders/Draw-vert.spv", SHADER_STAGE_VERTEX_SHADER, "main", true));
	assert(testProgramSPIR.CreateVKShaderModule(device, SHADER_ASSETS_FOLDER"TestShaders/Draw-frag.spv", SHADER_STAGE_FRAGMENT_SHADER, "main", true));

	//
	//PCO
	//
	assert(graphicsPCO.InitVKPipelineCache(device));

	//
	//Pipeline state
	//
	VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
	VkDynamicState dynamicStates[2];
	dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
	dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;
	EngineAPI::Statics::VulkanStates::GeneratePipelineDynamicStatesCreateStruct(&dynamicStateInfo, &dynamicStates[0], 2);

	VkPipelineVertexInputStateCreateInfo vertexInputStateInfo = {};
	EngineAPI::Statics::VulkanStates::GeneratePipelineVertexInputCreateStruct(&vertexInputStateInfo,
		vb.GetInputBindingDescription(), 1,
		vb.GetInputAttributesDescriptions(), vb.GetInputAttributesDescriptionsCount());

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
	EngineAPI::Statics::VulkanStates::GeneratePipelineInputAssemblyCreateStruct(&inputAssemblyInfo, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);

	VkPipelineRasterizationStateCreateInfo rasterStateInfo = {};
	EngineAPI::Statics::VulkanStates::GeneratePipelineRasterStateCreateStruct(&rasterStateInfo,
		VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE, VK_TRUE);

	//TODO: Abstract this -> Perhaps no blending as a default (default params to the Genetrate*() function)?
	VkPipelineColorBlendAttachmentState colourBlendAttachmentStateInfo[1];
	colourBlendAttachmentStateInfo[0] = {};
	colourBlendAttachmentStateInfo[0].colorWriteMask = 0xF;
	colourBlendAttachmentStateInfo[0].blendEnable = VK_FALSE;
	colourBlendAttachmentStateInfo[0].alphaBlendOp = VK_BLEND_OP_ADD;
	colourBlendAttachmentStateInfo[0].colorBlendOp = VK_BLEND_OP_ADD;
	colourBlendAttachmentStateInfo[0].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colourBlendAttachmentStateInfo[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colourBlendAttachmentStateInfo[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colourBlendAttachmentStateInfo[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

	VkPipelineColorBlendStateCreateInfo colourBlendStateInfo = {};
	EngineAPI::Statics::VulkanStates::GeneratePipelineColourBlendCreateStruct(&colourBlendStateInfo,
		VK_FALSE, (VkLogicOp)0,
		&colourBlendAttachmentStateInfo[0], 1);

	//TODO: Abstract viewport struct
	VkViewport viewport = {};
	viewport.width = graphicsSubsystem->GetRenderingSwapchain()->GetSwapchainDimentions().width;
	viewport.height = graphicsSubsystem->GetRenderingSwapchain()->GetSwapchainDimentions().height;
	viewport.x = 0;
	viewport.y = 0;
	viewport.maxDepth = 1.0f; //VERIFY
	viewport.minDepth = 0.0f; //VERIFY

	//TODO: Get scissor rect from viewport struct???
	VkRect2D scissor = {};

	VkPipelineViewportStateCreateInfo viewportStateInfo = {};
	EngineAPI::Statics::VulkanStates::GeneratePipelineViewportStateCreateStruct(&viewportStateInfo,
		nullptr, 1, nullptr, 1); //VERIFY!

	VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo = {};
	EngineAPI::Statics::VulkanStates::GeneratePipelineDepthStencilStateCreateStruct(&depthStencilStateInfo,
		VK_TRUE, VK_TRUE); //VERIFY (Stencil front/back in particular + depth bounds!!!!)

	VkPipelineMultisampleStateCreateInfo multiSampleStateInfo = {};
#if GRAPHICS_CONFIG_ENABLE_MSAA
	EngineAPI::Statics::VulkanStates::GeneratePipelineMultisampleStateCreateStruct(&multiSampleStateInfo,
		GRAPHICS_CONFIG_MSAA_SAMPLE_COUNT);
#else
	EngineAPI::Statics::VulkanStates::GeneratePipelineMultisampleStateCreateStruct(&multiSampleStateInfo,
		VK_SAMPLE_COUNT_1_BIT);
#endif

	//TODO:
	VkPipelineTessellationStateCreateInfo tessStateInfo = {};
	

	PipelineStateDescription pipelineStateDesc = {};
	pipelineStateDesc.vertexInputStateInfo = &vertexInputStateInfo;
	pipelineStateDesc.inputAssemblyInfo = &inputAssemblyInfo;
	pipelineStateDesc.rasterStateInfo = &rasterStateInfo;
	pipelineStateDesc.colourBlendStateInfo = &colourBlendStateInfo;
	pipelineStateDesc.multiSampleStateInfo = &multiSampleStateInfo;
	pipelineStateDesc.dynamicState = &dynamicStateInfo;
	pipelineStateDesc.viewportStateInfo = &viewportStateInfo;
	pipelineStateDesc.depthStencilStateInfo = &depthStencilStateInfo;
	pipelineStateDesc.tessStateInfo = nullptr; //TODO

	assert(graphicsPipelineState.InitVKGraphicsPipelineState(device, &graphicsPCO, graphicsSubsystem->GetRenderPass(), &testProgramSPIR, &pipelineStateDesc, true));

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

	testProgramSPIR.Shutdown(true);

	//Shutdown pipeline data
	graphicsPCO.Shutdown();
	graphicsPipelineState.Shutdown();

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