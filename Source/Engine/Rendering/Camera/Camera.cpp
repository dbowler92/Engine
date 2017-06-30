#include "Camera.h"

using namespace EngineAPI::Rendering;

Camera::Camera()
{}

Camera::~Camera()
{}

void Camera::Init(EngineAPI::Rendering::Transform& initialTransform)
{
	//Copy transform
	cameraTransform = initialTransform;

	//Build view matrix initially
	RebuildViewMatrix();
}

void Camera::InitCameraProjectionPerspective(float fovY, float aspect, float nearZ, float farZ)
{
	projectionMatrix = glm::perspective(fovY, aspect, nearZ, farZ);
}


void Camera::InitCameraProjectionOrthographic(float left, float right, float bottom, float top, float nearZ, float farZ)
{
	projectionMatrix = glm::ortho(left, right, bottom, top, nearZ, farZ);
}

void Camera::RebuildViewMatrix()
{
	//Build view matrix based on camera transform
}