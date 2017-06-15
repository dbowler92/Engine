//TextureData.h
//Created 15/16/17
//Created By Daniel Bowler
//
//Represents raw texture data (eg: loaded from file)

#pragma once

#include <string>
#include <vector>

//Texture loading
#include <gli/gli.hpp> //NOTE: Preprocessor define required: _CRT_SECURE_NO_WARNINGS
#include "../../3rdParty/LodePNG/lodepng.h"

//Which texture loading API do we wish to use
enum TextureLoadingAPI
{
	TEXTURE_LOADING_API_LODE_PNG,
	TEXTURE_LOADING_API_GLI
};

namespace EngineAPI
{
	namespace Rendering
	{
		class TextureData
		{
		public:
			TextureData();
			~TextureData();

			//Shutdown the texture data once done with it
			void Shutdown();

			//Load texture from file. 
			bool LoadTextureFromFile(const char* filename, TextureLoadingAPI api);

		public:
			//Getters
			unsigned char* GetRawTextureData() { return rawTextureData.data(); };
			uint32_t GetTextureDataSize() { return textureSizeBytes; };
			uint32_t GetTextureWidth() { return imageWidth; };
			uint32_t GetTextureHeight() { return imageHeight; };
			uint32_t GetMipmapLevelsCount() { return mipmapLevelsCount; };

		protected:
			//API usage to load the texture data from file
			TextureLoadingAPI usedAPI;

			//Name of the file
			std::string filename;

			//Texel data
			std::vector<unsigned char> rawTextureData;

			//Texture data
			uint32_t imageWidth = 0;
			uint32_t imageHeight = 0;
			uint32_t mipmapLevelsCount = 0;
			uint32_t textureSizeBytes = 0; //Actual size of the texture loaded by API
		};
	};
};
