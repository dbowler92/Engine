#include "VulkanGraphicsManager.h"

#include <iostream>

using namespace EngineAPI::Graphics::Platform;

VulkanGraphicsManager::VulkanGraphicsManager()
{
	//Default clear colour
	swpachainClearColour.R = 0.0f;
	swpachainClearColour.G = 0.0f;
	swpachainClearColour.B = 0.0f;
	swpachainClearColour.A = 0.0f;
}

bool VulkanGraphicsManager::ShutdownSubsystem()
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager::ShutdownSubsystem()\n");

	//Shutdown render pass
	swapchainRenderPass.Shutdown();

	//Cleanup vulkan (reverse order to creation)
	renderingSwapchain.Shutdown();
	renderingDevice.Shutdown();
	renderingInstance.Shutdown();

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

	//
	//Init vulkan API
	//
	if (!renderingInstance.InitVKInstance(osWindow, appTitle, appVersionMajor, appVersionMinor, appVersionPatch))
		return false;
	if (!renderingDevice.InitVKPhysicalDevice(osWindow, &renderingInstance))
		return false;
	if (!renderingSwapchain.InitVKLogicalSurface(osWindow, &renderingInstance))
		return false;
	if (!renderingDevice.InitVKLogicalDeviceAndQueues(osWindow, &renderingInstance, renderingSwapchain.GetVKLogicalSurfaceKHR()))
		return false;
	if (!renderingDevice.InitVKMemoryAllocator(osWindow, &renderingInstance))
		return false;
	if (!renderingDevice.InitVKCommandBufferPools(osWindow, &renderingInstance))
		return false;
	if (!renderingSwapchain.InitVKSwapchain(osWindow, &renderingInstance, &renderingDevice))
		return false;

	//Init render pass
	if (!InitVKRenderPass())
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsManager::InitSubsystem() Error: Could not init VkRenderPass\n");
		return false;
	}

	//Init the framebuffers
	if (!renderingSwapchain.InitVKFramebuffers(&renderingDevice, &swapchainRenderPass))
		return false;

	//Init command buffers for each swapchain image
	if (!renderingSwapchain.InitVKSwapchainRenderPassInstanceCommandBuffers(&renderingDevice, &swapchainRenderPass, swpachainClearColour, swapchainDepthClearValue, swapchainStencilClearValue))
		return false;

	//Done
	return true;
}

bool VulkanGraphicsManager::OnResize(EngineAPI::OS::OSWindow* osWindow)
{
	//TEMP
	//renderingDevice.GetDeviceMemoryAllocator()->Debug_LongDump(DEBUG_DUMPS_FOLDER"PreOnResize");

	EngineAPI::Debug::DebugLog::PrintInfoMessage("VulkanGraphicsManager::OnResize()\n");

	ESize2D newSize;
	newSize.Width = osWindow->GetWindowWidth();
	newSize.Height = osWindow->GetWindowHeight();

	//Pass along resize event to those that care
	if (!renderingDevice.OnResize(newSize))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsManager::OnResize() Error: Device failed during resize event\n");
		return false;
	}
	if (!renderingSwapchain.OnResize(osWindow, &renderingInstance, &renderingDevice))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsManager::OnResize() Error: Swapcahin failed during resize event\n");
		return false;
	}

	//Recreate render pass
	swapchainRenderPass.Shutdown();
	InitVKRenderPass();

	//Reinit swpachain framebuffers and swapchain render pass instance cmd buffers
	//
	//Init the framebuffers
	if (!renderingSwapchain.InitVKFramebuffers(&renderingDevice, &swapchainRenderPass))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsManager::OnResize() Error: Could not reinit swapchain framebuffers during resize event\n");
		return false;
	}

	//Init command buffers for each swapchain image
	if (!renderingSwapchain.InitVKSwapchainRenderPassInstanceCommandBuffers(&renderingDevice, &swapchainRenderPass, swpachainClearColour, swapchainDepthClearValue, swapchainStencilClearValue))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsManager::OnResize() Error: Could not reinit swapchain render pass instance (cmd buffers) during resize event\n");
		return false;
	}

	//TEMP
	//renderingDevice.GetDeviceMemoryAllocator()->Debug_LongDump(DEBUG_DUMPS_FOLDER"PostOnResize");
}

bool VulkanGraphicsManager::SetSwapchainClearValues(UNorm32Colour colourBufferClear, float depthClear, uint32_t stencilClear)
{
	//Store new values
	swpachainClearColour = colourBufferClear;
	swapchainDepthClearValue = depthClear;
	swapchainStencilClearValue = stencilClear;

	//TODO: Recreate the swpachain command buffers with new clear
	//colours
	if (!renderingSwapchain.InitVKSwapchainRenderPassInstanceCommandBuffers(&renderingDevice, &swapchainRenderPass,
		swpachainClearColour, swapchainDepthClearValue, swapchainStencilClearValue))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsManager::SetSwapchainClearValues(): Could not rebuild the swapcahin render pass instance command buffers for new clear values\n");
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
	attachments[0].format = renderingSwapchain.GetSwpachainImageFormat();
	attachments[0].samples = (VkSampleCountFlagBits)msaaSamples;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;   //Not depth/stencil attachment
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; 
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;		
	attachments[0].flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;

	//Depth buffer. 
	attachments[1].format = renderingSwapchain.GetDepthTexture()->GetVkImageFormat();
	attachments[1].samples = (VkSampleCountFlagBits)msaaSamples;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	if (renderingSwapchain.GetDepthTexture()->DoesContainStencilComponent())
	{
		attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //VK_ATTACHMENT_LOAD_OP_LOAD;
		attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	}
	else
	{
		attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	}

	attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachments[1].flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;

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
	if (!swapchainRenderPass.InitVKRenderPass(&renderingDevice, &renderPassCreateInfo))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("VulkanGraphicsManager::InitVKRenderPass() Error: Could not create the render pass\n");
		return false;
	}

	//Done
	return true;
}