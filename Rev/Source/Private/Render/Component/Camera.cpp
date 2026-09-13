#include "Rev/Render/Component/Camera.h"
#include "Rev/Core/Assert.h"

namespace Rev
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
	if (mProjectionType != ProjectionType::Perspective) mProjectionType = ProjectionType::Perspective;
	RecalculateProjection();
}

void Camera::SetOrthographic(float size, float dnear, float dfar)
{
	mOrthographicSize = size;
	mOrthographicNear = dnear;
	mOrthographicFar = dfar;
	if (mProjectionType != ProjectionType::Orthographic) mProjectionType = ProjectionType::Orthographic;
	RecalculateProjection();
}

void Camera::SetAspectRatio(float ratio)
{
	if (mAspectRatio != ratio)
	{
		mAspectRatio = ratio;
		RecalculateProjection();
	}
}

void Camera::SetProjectionType(ProjectionType type)
{
	if (mProjectionType != type)
	{
		mProjectionType = type;
		RecalculateProjection();
	}
}

void Camera::RecalculateProjection()
{
	if (mProjectionType == ProjectionType::Perspective)
	{
		mProjection = Math::FMatrix4::Perspective(Math::Radians(mPerspectiveFOV), mAspectRatio, mPerspectiveNear, mPerspectiveFar);
	}
	else
	{
		float orthoLeft = -mOrthographicSize * mAspectRatio * 0.5f;
		float orthoRight = mOrthographicSize * mAspectRatio * 0.5f;
		float orthoBottom = -mOrthographicSize * 0.5f;
		float orthoTop = mOrthographicSize * 0.5f;

		mProjection = Math::FMatrix4::Othographic(orthoLeft, orthoRight, orthoBottom, orthoTop, mOrthographicNear, mOrthographicFar);
	}
}

}

namespace Rev
{
Camera::FCameraProjectionInfo Camera::GetProjectionInfo() const
{
	FCameraProjectionInfo Result;
	Result.Type = mProjectionType;
	Result.NearClip = mProjectionType == ProjectionType::Perspective ? mPerspectiveNear : mOrthographicNear;
	Result.FarClip = mProjectionType == ProjectionType::Perspective ? mPerspectiveFar : mOrthographicFar;
	Result.VerticalFOV = mPerspectiveFOV;
	Result.AspectRatio = mAspectRatio;
	Result.OrthographicSize = mOrthographicSize;
	return Result;
}
}
