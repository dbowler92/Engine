//VulkanSampler2D.h
//Created 08/06/17
//Created By Daniel Bowler
//
//Represents a Vulkan image that can be sampled in a shader. Eg: Diffuse texture, normal map etc.

#pragma once

//Base class
#include "../../Texture/Texture.h"

//Does need to know about the device to create it
#include "../../../Graphics/Device/RenderDevice.h"

//May use staging buffers to transfer data from CPU to GPU memory
#include "../../../Graphics/StagingBuffer/StagingBuffer.h"

//Texture loading
#include <gli/gli.hpp> //NOTE: Preprocessor define required: _CRT_SECURE_NO_WARNINGS
#include "../../../3rdParty/LodePNG/lodepng.h"

//Which texture loading API do we wish to use
enum TextureLoadingAPI
{
	TEXTURE_LOADING_API_LODE_PNG,
	TEXTURE_LOADING_API_GLI
};

//Tiling mode
enum TextureTilingMode
{
	TEXTURE_TILING_MODE_LINEAR,
	TEXTURE_TILING_MODE_OPTIMAL
};

namespace EngineAPI
{
	namespace Rendering
	{
		namespace Platform
		{
			class VulkanSampler2D : public EngineAPI::Rendering::Texture
			{
			public:
				VulkanSampler2D(RenderingResourceType type) : Texture(type) {};
				virtual ~VulkanSampler2D() = 0 {};

				//Shutsdown the sampler2D. 
				void Shutdown();

				//1) Inits the VkImage which represents this sampler2D object
				bool InitVKSampler2DFromFile(EngineAPI::Graphics::RenderDevice* renderingDevice, 
					const char* filename, TextureLoadingAPI textureLoadingAPI, TextureTilingMode tilingMode, bool isDynamicTexture,
					VkFormat desiredImageFormat = VK_FORMAT_R8G8B8A8_UNORM, VkImageUsageFlags desiredImageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT);

				//2) Alloc memory for this texture
				bool AllocAndBindVKSampler2D(EngineAPI::Graphics::RenderDevice* renderingDevice,
					EngineAPI::Graphics::DeviceMemoryStore* optionalDeviceStore = nullptr);

				//3) Inits the sampler2D image views && layout. 
				bool InitVKSampler2DLayoutAndViews(EngineAPI::Graphics::RenderDevice* renderingDevice);
			
			public:
				//Returns the shader sampler image view
				VkImageView GetVKShaderSamplerImageView() { return vkShaderSamplerView; };

			protected:
				//Image view -> Used to bind the image as shader input
				VkImageView vkShaderSamplerView = VK_NULL_HANDLE;

				//Command buffers
				VkCommandBuffer vkSampler2DTextureImageLayoutCmdBuffer = VK_NULL_HANDLE;

				//Image loading - Dynamically created since we can only load a gli::texture2D
				//when the constructor is called. 
				gli::texture2D* gliTexture2D = nullptr;
				std::vector<unsigned char>lodePNGtextureBuffer;
				
				//Staging buffer -> Used to transfer data from host visible memory to 
				//device local memory
				EngineAPI::Graphics::StagingBuffer stagingBuffer;

			private:
				//Internal init
				bool InitVKSampler2DLayout(EngineAPI::Graphics::RenderDevice* renderingDevice);
				bool InitVKSampler2DViews(EngineAPI::Graphics::RenderDevice* renderingDevice);

				//Writes the parsed data to memory
				bool WriteParsedTextureDataToMemory(uint8_t* data);

				//TODO: Auto generation of mip levels
				bool AutoGenerateMips(uint8_t* textureData);

			private:
				//Clears the GLI && lodePNG data
				void CleanupGLIData();
				void CleanupLodePNGData();
			};
		};
	};
};