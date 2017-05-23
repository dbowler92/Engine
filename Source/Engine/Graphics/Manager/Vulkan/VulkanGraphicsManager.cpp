#include "VulkanGraphicsManager.h"

#include <iostream>

using namespace EngineAPI::Graphics::Platform;

bool VulkanGraphicsManager::ShutdownSubsystem()
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager::ShutdownSubsystem()\n");

	//Cleanup render pass
	vkDestroyRenderPass(renderingDevice->GetVKLogicalDevice(), vkRenderPass, nullptr);
	vkRenderPass = VK_NULL_HANDLE;

	//Cleanup vulkan (reverse order to creation)
	renderingSwapchain->Shutdown();
	renderingDevice->Shutdown();
	renderingInstance->Shutdown();

	//Cleanup memory
	if (renderingSwapchain)
		delete renderingSwapchain;
	if (renderingDevice)
		delete renderingDevice;
	if (renderingInstance)
		delete renderingInstance;

	//Done
	return true;
}

bool VulkanGraphicsManager::InitSubsystem(EngineAPI::OS::OSWindow* osWindow,
	ECHAR* appTitle, int appVersionMajor, int appVersionMinor, int appVersionPatch,
	unsigned screenWidth, unsigned screenHeight)
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager::InitSubsystem()\n");

#if SHOULD_PRINT_GRAPHICS_INIT_INFO
	char vkInfoMsg[256];

	//Engine version
	sprintf(&vkInfoMsg[0], "VulkanGraphicsManager: Game Engine Build: %s. Version: %i, %i, %i\n",
		ENGINE_NAME, ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, ENGINE_VERSION_PATCH);
	EngineAPI::Debug::DebugLog::PrintInfoMessage(vkInfoMsg);

	//App Version
	sprintf(&vkInfoMsg[0], "VulkanGraphicsManager: Application Build: %s. Version: %i, %i, %i\n",
		appTitle, appVersionMajor, appVersionMinor, appVersionPatch);
	EngineAPI::Debug::DebugLog::PrintInfoMessage(vkInfoMsg);

	//Minimum vulkan API required
	sprintf(&vkInfoMsg[0], "VulkanGraphicsManager: Minimum Vulkan API Supported: %i, %i, %i\n",
		ENGINE_CONFIG_MINIMUM_VULKAN_API_MAJOR, ENGINE_CONFIG_MINIMUM_VULKAN_API_MINOR, ENGINE_CONFIG_MINIMUM_VULKAN_API_PATCH);
	EngineAPI::Debug::DebugLog::PrintInfoMessage(vkInfoMsg);
#endif

	//Alloc
	renderingInstance = GE_NEW EngineAPI::Graphics::RenderInstance();
	renderingDevice = GE_NEW EngineAPI::Graphics::RenderDevice();
	renderingSwapchain = GE_NEW EngineAPI::Graphics::Swapchain();

	//
	//Init vulkan API
	//
	if (!renderingInstance->InitVKInstance(osWindow, appTitle, appVersionMajor, appVersionMinor, appVersionPatch))
		return false;
	if (!renderingDevice->InitVKPhysicalDevice(osWindow, renderingInstance))
		return false;
	if (!renderingSwapchain->InitVKLogicalSurface(osWindow, renderingInstance))
		return false;
	if (!renderingDevice->InitVKLogicalDeviceAndQueues(osWindow, renderingInstance, renderingSwapchain->GetVKLogicalSurfaceKHR()))
		return false;
	if (!renderingDevice->InitVKMemoryAllocator(osWindow, renderingInstance))
		return false;
	if (!renderingDevice->InitVKCommandBufferPools(osWindow, renderingInstance))
		return false;
	if (!renderingSwapchain->InitVKSwapchain(osWindow, renderingInstance, renderingDevice))
		return false;

	//Init render pass
	if (!InitVKRenderPass())
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsManager::InitSubsystem() Error: Could not init VkRenderPass\n");
		return false;
	}

	//Done
	return true;
}

bool VulkanGraphicsManager::InitVKRenderPass()
{
#if GRAPHICS_CONFIG_ENABLE_MSAA
	uint32_t msaaSamples = GRAPHICS_CONFIG_MSAA_SAMPLE_COUNT;
#else
	uint32_t msaaSamples = 1;
#endif

	//Attach swapchain colour image and depth buffer as 
	//attachment to render pass instance
	VkAttachmentDescription attachments[2];

	//Colour buffer
	attachments[0].flags = 0;
	attachments[0].format = renderingSwapchain->GetSwpachainFormat();
	attachments[0].samples = (VkSampleCountFlagBits)msaaSamples;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//Depth buffer. 
	attachments[1].flags = 0;
	attachments[1].format = renderingSwapchain->GetDepthTexture()->GetVkImageFormat();
	attachments[1].samples = (VkSampleCountFlagBits)msaaSamples;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	if (renderingSwapchain->GetDepthTexture()->DoesContainStencilComponent())
	{
		attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //VK_ATTACHMENT_LOAD_OP_LOAD;
		attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	}
	else
	{
		attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	}

	attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//Define the colour buffer attachment binding point. 
	VkAttachmentReference colourReference = {};
	colourReference.attachment = 0;
	colourReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//And the same for the depth buffer
	VkAttachmentReference depthReference = {};
	depthReference.attachment = 1;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//Subpass
	VkSubpassDescription subpass = {};
	subpass.flags = 0;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.inputAttachmentCount = 0;
	subpass.pInputAttachments = nullptr;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colourReference;
	subpass.pResolveAttachments = nullptr;
	subpass.pDepthStencilAttachment = &depthReference;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = nullptr;

	//Create render pass. 
	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.pNext = nullptr;
	renderPassCreateInfo.attachmentCount = 2; //Colour, Depth
	renderPassCreateInfo.pAttachments = attachments;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = 0;
	renderPassCreateInfo.pDependencies = nullptr;

	//Create it
	VkResult result = vkCreateRenderPass(renderingDevice->GetVKLogicalDevice(), &renderPassCreateInfo, nullptr, &vkRenderPass);
	if (result != VK_SUCCESS)
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsManager::InitVKRenderPass() Error: vkCreateRenderPass() failed\n");
		return false;
	}

	//Done
	return true;
}

void VulkanGraphicsManager::BeginFrame()
{

}

void VulkanGraphicsManager::EndFrame()
{

}