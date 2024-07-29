#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"

namespace Rev
{

class Camera
{
public:
	enum class ProjectionType { Perspective = 0, Orthographic = 1 };
public:
	REV_API Camera();
	REV_API virtual ~Camera() = default;

	REV_API void SetPerspective(float fov, float dnear, float dfar); //currently use vertical fov
	REV_API void SetOrthographic(float size, float dnear, float dfar);
	REV_API void SetAspectRatio(float ratio);

	REV_API ProjectionType GetProjectionType() const { return mProjectionType; }
	REV_API void SetProjectionType(ProjectionType type);
	REV_API Math::FMatrix4 GetProjectionMatrix() const { return mProjection; }

private:
	void RecalculateProjection();
private:
	ProjectionType mProjectionType = ProjectionType::Perspective;

	float mPerspectiveFOV = 45.0f;
	float mPerspectiveNear = 0.01f, mPerspectiveFar = 10000.0f;

	float mOrthographicSize = 10.0f;
	float mOrthographicNear = -1.0f, mOrthographicFar = 1.0f;

	float mAspectRatio = 1.0f;

	Math::FMatrix4 mProjection = Math::FMatrix4(1.0f);
};

}