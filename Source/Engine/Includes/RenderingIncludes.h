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

//Texture objects:
#include "../Rendering/DepthTexture/DepthTexture.h"

//Buffer objects
#include "../Rendering/VertexBuffer/VertexBuffer.h"
#include "../Rendering/IndexBuffer/IndexBuffer.h"
#include "../Rendering/UniformBuffer/UniformBuffer.h"

//Shaders && programs/materials
#include "../Graphics/Program/Program.h"
#include "../Graphics/Shader/Shader.h"

//Pipeline managements
#include "../Graphics/GraphicsPipelineCache/GraphicsPipelineCache.h"
#include "../Graphics/GraphicsPipelineState/GraphicsPipelineState.h"
#include "../Graphics/GraphicsPipelineLayout/GraphicsPipelineLayout.h"

//Descriptors
#include "../Graphics/DescriptorSet/DescriptorSet.h"