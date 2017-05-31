//RenderingIncludes.h
//Created 19/05/17
//Created By Daniel Bowler
//
//A one stop shop for all rendering classes that you
//may want to #include

#pragma once

//Texture objects:
#include "../Rendering/DepthTexture/DepthTexture.h"

//Buffer objects
#include "../Rendering/VertexBuffer/VertexBuffer.h"

//Shaders && programs/materials
#include "../Graphics/Program/Program.h"
#include "../Graphics/Shader/Shader.h"

//Pipeline managements
#include "../Graphics/GraphicsPipelineCache/GraphicsPipelineCache.h"
#include "../Graphics/GraphicsPipelineState/GraphicsPipelineState.h"

//Drawable objects
#include "../Rendering/DrawableObject/DrawableObject.h"