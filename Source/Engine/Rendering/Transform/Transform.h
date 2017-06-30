//Transform.h
//Created 22/06/17
//Created By Daniel Bowler
//
//Data representing a transform in the game - eg: World/model matrix

#pragma once

//Math
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace EngineAPI
{
	namespace Rendering
	{
		class Transform
		{
		public:
			Transform();
			~Transform();

			//Getters
			glm::vec3 GetPosition() { return position; };
			glm::vec3 GetRotations() { return rotations; };
			glm::vec3 GetScales() { return scales; };

			//Setters
			void SetTransform(glm::vec3 p, glm::vec3 r, glm::vec3 s) { position = p; rotations = r; scales = s; };
			void SetPosition(glm::vec3 p)  { position = p; };
			void SetRotations(glm::vec3 r) { rotations = r; };
			void SetScales(glm::vec3 s)    { scales = s; };

			//Generates a transformation matrix from position, rotations and scales
			glm::mat4 GenerateTransformMat4();

		protected:
			//Transform properties
			glm::vec3 position;
			glm::vec3 rotations;
			glm::vec3 scales;
		};
	};
};