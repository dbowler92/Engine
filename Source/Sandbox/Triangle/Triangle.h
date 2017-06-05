//Triangle.h
//Created 01/06/17
//Created By Daniel Bowler
//
//Simple triangle which will be rendered to the screen

#pragma once

//Base
#include "../../Engine/Rendering/DrawableObject/DrawableObject.h"

class Triangle : public EngineAPI::Rendering::DrawableObject
{
public:
	Triangle();
	~Triangle();

	void Shutdown() override;
	void GenerateRenderingCommands(EngineAPI::Graphics::GraphicsManager* graphicsSubsystem) override;
	
	void Init(EngineAPI::Graphics::GraphicsManager* graphicsSubsystem);

protected:
	EngineAPI::Rendering::VertexBuffer vb;
	EngineAPI::Graphics::Program testProgramSPIR;

	EngineAPI::Graphics::GraphicsPipelineState graphicsPipelineState;
};