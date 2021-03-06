//TexturedCube.h
//Created 05/06/17
//Created By Daniel Bowler
//
//Simple cube with a texture applied

#pragma once

//Base
#include "../../Engine/Rendering/DrawableObject/DrawableObject.h"

//Optimal vs linear tiling for texture...
#define TEXTURED_CUBE_DO_USE_OPTIMAL_TILING_FOR_SAMPLER true;

class TexturedCube : public EngineAPI::Rendering::DrawableObject
{
public:
	TexturedCube();
	~TexturedCube();

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

	EngineAPI::Rendering::TextureData textureFromFile;
	EngineAPI::Graphics::StagingBuffer textureStagingBuffer;

	EngineAPI::Graphics::SamplerState samplerState;
	EngineAPI::Rendering::Sampler2D sampler2D;

private:
	void InitSamplerOptimal(EngineAPI::Graphics::GraphicsManager* graphicsSubsystem);
	void InitSamplerLinear(EngineAPI::Graphics::GraphicsManager* graphicsSubsystem);
};