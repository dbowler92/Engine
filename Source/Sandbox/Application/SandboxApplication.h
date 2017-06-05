//SandboxApplication.h
//Created 23/07/16
//Created by Daniel Bowler
//
//Game specific instance of the engines Application (layer) class. Here, we will
//provide implementations for the virtual functions defined in ...Engine/Application.h

#pragma once

#include "../../Engine/Application/Application.h" //Base class

//Rendering includes
#include "../../Engine/Includes/RenderingIncludes.h"

//Test rendering objects
#include "../Triangle/Triangle.h"
#include "../Quad/Quad.h" //IB && VB
#include "../TexturedCube/TexturedCube.h"

class SandboxApplication : public EngineAPI::Core::Application
{ 
	HIDE_COPY_ASSIGNMENT(SandboxApplication)
public:  
	SandboxApplication();
	~SandboxApplication() {};
	 
	//
	//Virtual functions. Game specific implementations
	//
	virtual ECHAR* GetGameTitle();

	//Inits sandbox game
	virtual bool InitApplication();

	//Closes the sandbox game
	virtual bool ShutdownApplication();

	//Updates and renders the scene
	virtual void UpdateScene(float dt);
	virtual void RenderScene();

protected:
	//Rendering object
	Triangle tri;
	Quad quad;
	TexturedCube tCube;

	bool doRenderTriangle = true;
}; 