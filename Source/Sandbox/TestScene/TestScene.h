//TestScene.h
//Created 30/06/17
//Created By Daniel Bowler
//
//Simple test scene used when creating the engine

#pragma once

//Parent class
#include "../../../Engine/Gameplay/Scene/Scene.h"

//Test rendering objects
#include "../Triangle/Triangle.h"
#include "../Quad/Quad.h" //IB && VB
#include "../TexturedCube/TexturedCube.h"
#include "../PushConstantCube/PushConstantCube.h"

class TestScene : public EngineAPI::Gameplay::Scene
{
public:
	TestScene() {};
	~TestScene() {};

	//Load and unload
	bool OnAddToSceneManagerList() override;
	bool OnSceneBecomeActive() override;
	bool OnSceneBecomeDeactive() override;
	bool OnRemoveFromSceneManagerList() override;
	bool OnEngineShutdown() override;

	//Rendering and updating the scene - NOTE: I will add
	//an update/render loop thats used internally. 
	bool OnSceneUpdate(float dt) override;
	bool OnSceneRender() override;

private:
	//Rendering object
	Triangle tri;
	Quad quad;
	TexturedCube tCube;
	PushConstantCube pConstCube;

};
