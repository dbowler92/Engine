//Quad.h
//Created 02/06/17
//Created By Daniel Bowler
//
//Represents a full screen quad -> Demonstrates index buffers

#pragma once

//Base
#include "../../Engine/Rendering/DrawableObject/DrawableObject.h"

class Quad : public EngineAPI::Rendering::DrawableObject
{
public:
	Quad();
	~Quad();

	void Shutdown() override;
	void GenerateRenderingCommands(EngineAPI::Graphics::GraphicsManager* graphicsSubsystem) override;

	void Init(EngineAPI::Graphics::GraphicsManager* graphicsSubsystem);

protected:
	EngineAPI::Rendering::VertexBuffer vb;
	EngineAPI::Rendering::IndexBuffer ib;

	EngineAPI::Graphics::Program testProgramSPIR;

	EngineAPI::Graphics::GraphicsPipelineCache graphicsPCO;
	EngineAPI::Graphics::GraphicsPipelineState graphicsPipelineState;
};