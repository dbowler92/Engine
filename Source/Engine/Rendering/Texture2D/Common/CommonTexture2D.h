//CommonTexture2D.h
//Created 29/04/17
//Created By Daniel Bowler
//
//Interface that each platform specific implementation of texture2D objects
//will inherit from

#pragma once

//Debug
#include "../../../Debug/Log/DebugLog.h"

//Device used to create this...
#include "../../../Graphics/Device/RenderDevice.h"

namespace EngineAPI
{
	namespace Rendering
	{
		namespace Interface
		{
			class CommonTexture2D
			{
			public:
				CommonTexture2D() {};
				virtual ~CommonTexture2D() = 0 {};

				//Shutdown function
				virtual void Shutdown() = 0;

				//TODO: General init function. Taking the format, usage, etc etc etc. 
				//virtual bool InitTexture2D(EngineAPI::Graphics::RenderDevice* renderingDevice) = 0;
			};
		};
	};
};