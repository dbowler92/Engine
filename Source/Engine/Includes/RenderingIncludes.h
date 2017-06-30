//RenderingIncludes.h
//Created 19/05/17
//Created By Daniel Bowler
//
//A one stop shop for all rendering classes that you
//may want to #include

#pragma once

//Math
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

//Texture loading
#include "../Rendering/TextureData/TextureData.h"

//Texture objects:
#include "../Rendering/DepthTexture/DepthTexture.h"
#include "../Rendering/Sampler2D/Sampler2D.h"

//Buffer objects
#include "../Rendering/VertexBuffer/VertexBuffer.h"
#include "../Rendering/IndexBuffer/IndexBuffer.h"
#include "../Rendering/UniformBuffer/UniformBuffer.h"

//Shaders && programs/materials
#include "../Graphics/Program/Program.h"
#include "../Graphics/Shader/Shader.h"

//Sampler state
#include "../Graphics/SamplerState/SamplerState.h"

//Pipeline managements
#include "../Graphics/GraphicsPipelineCache/GraphicsPipelineCache.h"
#include "../Graphics/GraphicsPipelineState/GraphicsPipelineState.h"
#include "../Graphics/GraphicsPipelineLayout/GraphicsPipelineLayout.h"

//Descriptors
#include "../Graphics/DescriptorSet/DescriptorSet.h"

//Push constants
#include "../Graphics/PushConstantBinding/PushConstantBinding.h"