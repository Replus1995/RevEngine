#include "pinepch.h"
#include "Camera.h"
#include "Pine/Core/Assert.h"

namespace Pine
{

Camera::Camera()
{
	RecalculateProjection();
}

void Camera::SetPerspective(float fov, float dnear, float dfar)
{
	mPerspectiveFOV = fov;
	mPerspectiveNear = dnear;
	mPerspectiveFar = dfar;
	SetProjectionType(ProjectionType::Perspective);
}

void Camera::SetOrthographic(float size, float dnear, float dfar)
{
	mOrthographicSize = size;
	mOrthographicNear = dnear;
	mOrthographicFar = dfar;
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
		mProjection = FMaths::Perspective(mPerspectiveFOV, mAspectRatio, mPerspectiveNear, mPerspectiveFar);
	}
	else
	{
		float orthoLeft = -mOrthographicSize * mAspectRatio * 0.5f;
		float orthoRight = mOrthographicSize * mAspectRatio * 0.5f;
		float orthoBottom = -mOrthographicSize * 0.5f;
		float orthoTop = mOrthographicSize * 0.5f;

		mProjection = FMaths::Othographic(orthoLeft, orthoRight, orthoBottom, orthoTop, mOrthographicNear, mOrthographicFar);
	}
}

}