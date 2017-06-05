//TexturedCube.h
//Created 05/06/17
//Created By Daniel Bowler
//
//Simple cube with a texture applied

#pragma once

//Base
#include "../../Engine/Rendering/DrawableObject/DrawableObject.h"

class TexturedCube : public EngineAPI::Rendering::DrawableObject
{
public:
	TexturedCube();
	~TexturedCube();

	void Shutdown() override;
	void GenerateRenderingCommands(EngineAPI::Graphics::GraphicsManager* graphicsSubsystem) override;

	void Init(EngineAPI::Graphics::GraphicsManager* graphicsSubsystem);

protected:
	EngineAPI::Rendering::VertexBuffer vb;
	EngineAPI::Rendering::IndexBuffer ib;

	EngineAPI::Rendering::UniformBuffer uniformBuffer;
	glm::mat4x4 uniformBufferMatrixData;

	EngineAPI::Graphics::Program testProgramSPIR;

	EngineAPI::Graphics::GraphicsPipelineState graphicsPipelineState;
	EngineAPI::Graphics::GraphicsPipelineLayout graphicsPipelineLayout;

	EngineAPI::Graphics::DescriptorSet descriptorSet;
};