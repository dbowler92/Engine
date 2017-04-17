//D3D11GraphicsManager.h
//Created 16/04/17
//Created By Daniel Bowler
//
//Represents the D3D11 API implementation of the common graphics manager. Sets up D3D11
//for us and interacts with the OS window provided
//
//Windows only.

#pragma once

//Shared interface
#include "../Common/CommonGraphicsManager.h"

//D3D11 header(s). 
//

namespace EngineAPI
{
	namespace Graphics
	{
		namespace Platform
		{
			class D3D11GraphicsManager : public EngineAPI::Graphics::Interface::CommonGraphicsManager
			{
			public:
				D3D11GraphicsManager() {};

				//Inits the graphics subsystem / manager (D3D11)
				bool InitSubsystem(ECHAR* appTitle,
					int appVersionMajor, int appVersionMinor, int appVersionPatch,
					unsigned screenWidth, unsigned screenHeight) override;

				//Shutsdown the graphics manager (D3D11)
				bool ShutdownSubsystem() override;
			};
		};
	};
};
