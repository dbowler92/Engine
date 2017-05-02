//CommonTextureResource.h
//Created 29/04/17
//Created By Daniel Bowler
//
//Base class for all texture/image types. 

#pragma once

//Debug
#include "../../../Debug/Log/DebugLog.h"

namespace EngineAPI
{
	namespace Rendering
	{
		namespace Interface
		{
			class CommonTextureResource
			{
			public:
				CommonTextureResource() {};
				virtual ~CommonTextureResource() = 0 {};

				//Shutdown - in this case, classes like Texture2D *must* call the 
				//super version of the Shutdown function (and so on) so that the underlying
				//TextureResource will be released
				virtual void Shutdown() = 0 {};

			};
		};
	};
};
