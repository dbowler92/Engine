//CommonOSWindow.h
//Created 19/04/17
//Created By Daniel Bowler
//
//Shared interface that each abstraction of the OS window will implement

#pragma once

namespace EngineAPI
{
	namespace OS
	{
		namespace Interface
		{
			class CommonOSWindow
			{
			public:
				CommonOSWindow() {};
				virtual ~CommonOSWindow() = 0 {};

				//Setters & getters
				void UpdateWindowWidth(unsigned w) { windowWidth = w; };
				void UpdateWindowHeight(unsigned h) { windowHeight = h; };

				unsigned GetWindowWidth() { return windowWidth; };
				unsigned GetWindowHeight() { return windowHeight; };

			protected:
				//Window size
				unsigned windowWidth;
				unsigned windowHeight;
			};
		};
	};
};