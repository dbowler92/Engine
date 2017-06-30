//Camera.h
//Created 22/06/17
//Created By Daniel Bowler
//
//Represents a virtual camera in the scene - maintains and manages the
//view/proj matrices. 

#pragma once

//Has a transform
#include "../Transform/Transform.h"

namespace EngineAPI
{
	namespace Rendering
	{
		class Camera
		{
		public:
			Camera();
			~Camera();

			//Inits the camera with an initial transform in world space.
			void Init(EngineAPI::Rendering::Transform& initialTransform);
			
			//Sets the projection mode + settings
			void InitCameraProjectionPerspective(float fovY, float aspect, float nearZ, float farZ);
			void InitCameraProjectionOrthographic(float left, float right, float bottom, float top, float nearZ, float farZ);

			//Rebuilds view matrix -> View matrix is cached internally
			void RebuildViewMatrix();

			//Getters: 
			//
			//For the matrices, returns the cached view/proj matrices -> Thus, ensure to 
			//call RebuildViewMatrix() each time the camera moves, rotates. 
			Transform GetTransform() { return cameraTransform; };
			glm::mat4 GetViewMatrix() { return viewMatrix; };
			glm::mat4 GetProjectionMatrix() { return projectionMatrix; };

		protected:
			//Camera transform (Used to generate view matrix)
			Transform cameraTransform;

			//Camera matrices
			glm::mat4 viewMatrix;
			glm::mat4 projectionMatrix;
		};
	};
};