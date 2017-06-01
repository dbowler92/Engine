#include "Triangle.h"

//Graphics subsystem include
#include "../../Engine/Graphics/Manager/GraphicsManager.h"

Triangle::Triangle()
{}

Triangle::~Triangle()
{}

void Triangle::Shutdown()
{
	//Shutdown rendering data
	vb.Shutdown();
	testProgramSPIR.Shutdown(true);
	graphicsPCO.Shutdown();
	graphicsPipelineState.Shutdown();
}

void Triangle::Init(EngineAPI::Graphics::GraphicsManager* graphicsSubsystem)
{
	//Device used to create things. 
	EngineAPI::Graphics::RenderDevice* device = graphicsSubsystem->GetRenderingDevice();

	//
	//VB
	//
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

	//
	//Alloc VB
	//
	assert(vb.AllocAndBindVKVertexBuffer(device, &vbLayout, &triangleData[0], customStoreForVB));

	//
	//Program
	//
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
	viewport.minDepth = 0.0f; //VERIFY
	viewport.maxDepth = 1.0f; //VERIFY

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
}

void Triangle::GenerateRenderingCommands(EngineAPI::Graphics::GraphicsManager* graphicsSubsystem)
{
	//Do we need to insert render commands?
	if (!graphicsSubsystem->GetCurrentRenderPassInstance()->GetRenderPassInstanceCommandBufferDirtyFlag())
		return;

	//Render pass instance command buffer object which we record rendering commands 
	//in to. 
	VkCommandBuffer cmdBuffer = graphicsSubsystem->GetCurrentRenderPassInstance()->GetVKRenderPassInstanceCommandBuffer();
	
	//Swapchain -> We need its dimentions. 
	EngineAPI::Graphics::Swapchain* swapchain = graphicsSubsystem->GetRenderingSwapchain();

	//Bind pipeline
	EngineAPI::Statics::VulkanCommands::VKCMD_BindGraphicsPipeline(cmdBuffer, graphicsPipelineState.GetVKPipelineHandle());

	//Bind VB
	const VkDeviceSize offsets[1] = { 0 };
	VkBuffer vertexBuffers[1] = { vb.GetVKBufferHandle() };
	EngineAPI::Statics::VulkanCommands::VKCMD_BindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);

	//Define (Dynamic) viewport
	VkViewport viewport = {};
	viewport.width = (float)swapchain->GetSwapchainDimentions().width;
	viewport.height = (float)swapchain->GetSwapchainDimentions().height;
	viewport.minDepth = (float) 0.0f;
	viewport.maxDepth = (float) 1.0f;
	viewport.x = 0;
	viewport.y = 0;
	EngineAPI::Statics::VulkanCommands::VKCMD_DynamicallySetViewports(cmdBuffer, 0, 1, &viewport);

	//Define (Dynamic) scissors 
	VkRect2D scissor;
	scissor.extent.width = swapchain->GetSwapchainDimentions().width;
	scissor.extent.height = swapchain->GetSwapchainDimentions().height;
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	EngineAPI::Statics::VulkanCommands::VKCMD_DynamicallySetScissors(cmdBuffer, 0, 1, &scissor);

	//Issue draw command (1 instance)
	EngineAPI::Statics::VulkanCommands::VKCMD_Draw(cmdBuffer, 3, 1, 0, 0);
}

