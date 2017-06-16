#include "TextureData.h"

//Debug
#include "../../Debug/Log/DebugLog.h"

using namespace EngineAPI::Rendering;

TextureData::TextureData()
{}

TextureData::~TextureData()
{
	Shutdown();
}

void TextureData::Shutdown()
{
	if (rawTextureData.size() > 0)
		rawTextureData.clear();
}

bool TextureData::LoadTextureFromFile(const char* filename, TextureLoadingAPI api)
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("TextureData::LoadTextureFromFile() Loading file: ");
	EngineAPI::Debug::DebugLog::PrintMessage(filename);
	EngineAPI::Debug::DebugLog::PrintMessage("\n");

	//Cache info
	usedAPI = api;
	this->filename = filename;

	//Load file
	if (usedAPI == TEXTURE_LOADING_API_GLI)
	{
		//GLI
		//
		//Load the file
		gli::texture2D gliTexture2D = gli::texture2D(gli::load(filename));
		assert(!gliTexture2D.empty());

		//Get image dimensions at the top sub-resource level
		imageWidth = (uint32_t)gliTexture2D[0].dimensions().x;
		imageHeight = (uint32_t)gliTexture2D[0].dimensions().y;

		//Mip levels in the parsed image
		mipmapLevelsCount = gliTexture2D.levels();

		//Raw image size
		textureSizeBytes = gliTexture2D.size();

		//Raw image data - TODO: Really don't like the load in to one buffer then memcpy
		//to a seperate buffer....
		rawTextureData.resize(textureSizeBytes);
		memcpy(rawTextureData.data(), gliTexture2D.data(), textureSizeBytes);
	}
	if (usedAPI == TEXTURE_LOADING_API_LODE_PNG)
	{
		//lodePNG
		unsigned error = lodepng::decode(rawTextureData, imageWidth, imageHeight, filename);
		if (error != 0)
		{
			EngineAPI::Debug::DebugLog::PrintErrorMessage("TextureData::LoadTextureFromFile() Error: Could not decode PNG (lodePNG)\n");
			return false;
		}

		//Mip levels
		mipmapLevelsCount = 1;

		//Texture size
		textureSizeBytes = rawTextureData.size(); //Verify
	}

	//TEMP: 
	assert(mipmapLevelsCount == 1);

	//Done
	return true;
}