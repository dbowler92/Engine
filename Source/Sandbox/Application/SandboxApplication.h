//SandboxApplication.h
//Created 23/07/16
//Created by Daniel Bowler
//
//Game specific instance of the engines Application (layer) class. Here, we will
//provide implementations for the virtual functions defined in ...Engine/Application.h

#pragma once

#include "../../Engine/Application/Application.h" //Base class

class SandboxApplication : public EngineAPI::Base::Application
{ 
	HIDE_COPY_ASSIGNMENT(SandboxApplication)
public:  
	SandboxApplication();
	 
	//
	//Virtual functions. Game specific implementations
	//
	TCHAR *VGetGameTitle() { return _T("Sandbox"); };
	TCHAR *VGetGameAppDirectory() { return _T("My Game Engine\\Sandbox\\1.0"); };
	HICON VGetIcon();
}; 