#pragma once
#include <memory>
#include <glm/glm.hpp>

namespace Pine
{

class Camera
{
public:
	enum class ProjectionType { Perspective = 0, Orthographic = 1 };
public:
	Camera();
	virtual ~Camera() = default;

	void SetPerspective(float fov, float near, float far); //currently use vertical fov
	void SetOrthographic(float size, float near, float far);
	void SetAspectRatio(float ratio);

	ProjectionType GetProjectionType() const { return mProjectionType; }
	void SetProjectionType(ProjectionType type);
	glm::mat4 GetProjectionMatrix() const { return mProjection; }

private:
	void RecalculateProjection();
private:
	ProjectionType mProjectionType = ProjectionType::Perspective;

	float mPerspectiveFOV = glm::radians(45.0f);
	float mPerspectiveNear = 0.01f, mPerspectiveFar = 1000.0f;

	float mOrthographicSize = 10.0f;
	float mOrthographicNear = -1.0f, mOrthographicFar = 1.0f;

	float mAspectRatio = 1.0f;

	glm::mat4 mProjection = glm::mat4(1.0f);
};

}