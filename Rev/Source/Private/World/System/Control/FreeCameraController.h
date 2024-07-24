#pragma once
#include "Rev/World/System/Control/CameraController.h"
#include "Rev/Events/Event.h"

namespace Rev
{

class REV_API FreeCameraController : public CameraController
{
public:
	FreeCameraController() = default;
	virtual ~FreeCameraController() = default;

	virtual void OnUpdate(float dt, Camera& camera, Math::FTransform& transform) override;
	virtual void OnEvent(Event& e, Camera& camera, Math::FTransform& transform) override;

private:
	void MousePan(Math::FTransform& transform, const Math::FVector3& delta);
	void MouseRotate(Math::FTransform& transform, const Math::FVector2& delta);
	//void MouseZoom(float delta);

private:
	bool mMoving{ false };
	Math::FVector2 mLastMousePos{ 0.0f, 0.0f };
};

}