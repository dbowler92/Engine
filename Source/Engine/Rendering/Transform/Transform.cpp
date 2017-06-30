#include "Transform.h"

using namespace EngineAPI::Rendering;

Transform::Transform()
{
	//Default
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	rotations = glm::vec3(0.0f, 0.0f, 0.0f);
	scales = glm::vec3(1.0f, 1.0f, 1.0f);
}

Transform::~Transform()
{}

glm::mat4 Transform::GenerateTransformMat4()
{
	glm::mat4 out(1.0f); //identity
	
	//TODO: Set matrix values individually rather than by
	//multiple matrix multiplications

	glm::mat4 tMatrix;
	glm::mat4 rXMatrix;
	glm::mat4 rYMatrix;
	glm::mat4 rZMatrix;
	glm::mat4 sMatrix;

	glm::translate(tMatrix, position);
	glm::rotate(rXMatrix, rotations.x, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::rotate(rYMatrix, rotations.y, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::rotate(rZMatrix, rotations.z, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::scale(sMatrix, scales);

	out = tMatrix * ((rXMatrix * rYMatrix * rZMatrix) * sMatrix);

	//Done
	return out;
}