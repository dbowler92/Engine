//CommonTextureResource.h
//Created 29/04/17
//Created By Daniel Bowler
//
//Base class for all texture/image types. 

#pragma once

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
			};
		};
	};
};
