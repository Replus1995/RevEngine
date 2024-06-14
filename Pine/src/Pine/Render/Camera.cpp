#include "pinepch.h"
#include "Camera.h"
#include "Pine/Core/Assert.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Pine
{

Camera::Camera()
{
	RecalculateProjection();
}

void Camera::SetPerspective(float fov, float near, float far)
{
	mPerspectiveFOV = fov;
	mPerspectiveNear = near;
	mPerspectiveFar = far;
	SetProjectionType(ProjectionType::Perspective);
}

void Camera::SetOrthographic(float size, float near, float far)
{
	mOrthographicSize = size;
	mOrthographicNear = near;
	mOrthographicFar = far;
	SetProjectionType(ProjectionType::Orthographic);
}

void Camera::SetAspectRatio(float ratio)
{
	mAspectRatio = ratio;
	RecalculateProjection();
}

void Camera::SetProjectionType(ProjectionType type)
{
	mProjectionType = type;
	RecalculateProjection();
}

void Camera::RecalculateProjection()
{
	if (mProjectionType == ProjectionType::Perspective)
	{
		mProjection = glm::perspective(mPerspectiveFOV, mAspectRatio, mPerspectiveNear, mPerspectiveFar);
	}
	else
	{
		float orthoLeft = -mOrthographicSize * mAspectRatio * 0.5f;
		float orthoRight = mOrthographicSize * mAspectRatio * 0.5f;
		float orthoBottom = -mOrthographicSize * 0.5f;
		float orthoTop = mOrthographicSize * 0.5f;

		mProjection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, mOrthographicNear, mOrthographicFar);
	}
}

}