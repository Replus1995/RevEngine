#pragma once
#include "Pine/World/System/Control/CameraController.h"
#include "Pine/Events/Event.h"

namespace Pine
{

class PINE_API FreeCameraController : public CameraController
{
public:
	FreeCameraController() = default;
	virtual ~FreeCameraController() = default;

	virtual void OnUpdate(float dt, Camera& camera, FTransform& transform) override;
	virtual void OnEvent(Event& e, Camera& camera, FTransform& transform) override;

private:
	void MousePan(FTransform& transform, const FVector2& delta);
	void MouseRotate(FTransform& transform, const FVector2& delta);
	//void MouseZoom(float delta);

private:
	bool mMoving{ false };
	FVector2 mLastMousePos{ 0.0f, 0.0f };
};

}