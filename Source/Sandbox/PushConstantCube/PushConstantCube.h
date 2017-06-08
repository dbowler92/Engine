//PushConstantCube.h
//Created 07/06/17
//Created By Daniel Bowler
//
//Simple cube - its colour will be defined by a constant in the shader. 
//The C++ will update this constant via push constants in Vulkan. 

#pragma once

//Base
#include "../../Engine/Rendering/DrawableObject/DrawableObject.h"

class PushConstantCube : public EngineAPI::Rendering::DrawableObject
{
public:
	PushConstantCube();
	~PushConstantCube();

	void Shutdown() override;

	void Init(EngineAPI::Graphics::GraphicsManager* graphicsSubsystem);

	void Update(float dt) override;
	void GenerateRenderingCommands(EngineAPI::Graphics::GraphicsManager* graphicsSubsystem) override;
	
protected:
	EngineAPI::Rendering::VertexBuffer vb;
	//EngineAPI::Rendering::IndexBuffer ib;

	EngineAPI::Rendering::UniformBuffer uniformBuffer;
	glm::mat4x4 uniformBufferMatrixData; //Fed in to the uniform buffer. 
	glm::mat4x4 world;
	glm::mat4x4 view;
	glm::mat4x4 proj;

	EngineAPI::Graphics::Program testProgramSPIR;

	EngineAPI::Graphics::GraphicsPipelineState graphicsPipelineState;
	EngineAPI::Graphics::GraphicsPipelineLayout graphicsPipelineLayout;

	EngineAPI::Graphics::DescriptorPool descriptorPool;
	EngineAPI::Graphics::DescriptorSet descriptorSet;

	EngineAPI::Graphics::PushConstantBinding pushConstantBinding; //static colour index && mix value. 
};