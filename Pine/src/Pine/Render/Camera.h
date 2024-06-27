#pragma once
#include "Pine/Math/Math.h"

namespace Pine
{

class PINE_API Camera
{
public:
	enum class ProjectionType { Perspective = 0, Orthographic = 1 };
public:
	Camera();
	virtual ~Camera() = default;

	void SetPerspective(float fov, float dnear, float dfar); //currently use vertical fov
	void SetOrthographic(float size, float dnear, float dfar);
	void SetAspectRatio(float ratio);

	ProjectionType GetProjectionType() const { return mProjectionType; }
	void SetProjectionType(ProjectionType type);
	FMatrix4 GetProjectionMatrix() const { return mProjection; }

private:
	void RecalculateProjection();
private:
	ProjectionType mProjectionType = ProjectionType::Perspective;

	float mPerspectiveFOV = FMaths::Radians(45.0f);
	float mPerspectiveNear = 0.01f, mPerspectiveFar = 1000.0f;

	float mOrthographicSize = 10.0f;
	float mOrthographicNear = -1.0f, mOrthographicFar = 1.0f;

	float mAspectRatio = 1.0f;

	FMatrix4 mProjection = FMatrix4(1.0f);
};

}