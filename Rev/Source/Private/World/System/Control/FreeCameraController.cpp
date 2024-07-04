#include "FreeCameraController.h"
#include "Rev/Core/Input.h"

namespace Rev
{
static constexpr float sPanFactor = 2.4f;
static constexpr float sPanSpeed = 1.0f;//0.0366f * (sPanFactor * sPanFactor) - 0.1778f * sPanFactor + 0.3021f;
static constexpr float sRotateSpeed = 0.1f;

void FreeCameraController::OnUpdate(float dt, Camera& camera, Math::FTransform& transform)
{
	bool bMouseRightDown = Input::MouseButtonDown(Mouse::ButtonRight);
	if (bMouseRightDown)
	{
		if (!mMoving)
		{
			mLastMousePos = Input::GetMousePosition();
			mMoving = true;
		}
		else
		{
			Math::FVector2 deltaPan(0.0f, 0.0f);
			if(Input::KeyDown(Key::W))
				deltaPan.Y += dt;
			if (Input::KeyDown(Key::S))
				deltaPan.Y += -dt;
			if (Input::KeyDown(Key::A))
				deltaPan.X += -dt;
			if (Input::KeyDown(Key::D))
				deltaPan.X += dt;
			deltaPan *= sPanSpeed;
			MousePan(transform, deltaPan);

			Math::FVector2 curMousePos = Input::GetMousePosition();
			Math::FVector2 deltaRotate= curMousePos - mLastMousePos;
			deltaRotate *= sRotateSpeed * -1.0f;
			mLastMousePos = curMousePos;
			MouseRotate(transform, deltaRotate);
		}
	}
	else
	{
		mMoving = false;
	}
}

void FreeCameraController::OnEvent(Event& e, Camera& camera, Math::FTransform& transform)
{
	/*if (e.GetEventType() == EventType::MouseScrolled)
	{
		MouseScrolledEvent* me = static_cast<MouseScrolledEvent*>(&e);
		float dy = me->GetYOffset() * 0.1f;
	}*/
}

void FreeCameraController::MousePan(Math::FTransform& transform, const Math::FVector2& delta)
{
	Math::FVector3 dRight = transform.Right() * delta.X;
	Math::FVector3 dForward = transform.Forward() * delta.Y;
	transform.Location += dRight + dForward;
}

void FreeCameraController::MouseRotate(Math::FTransform& transform, const Math::FVector2& delta)
{
	float yaw = transform.Rotation.Yaw + delta.X;
	float pitch = Math::Clamp(transform.Rotation.Pitch + delta.Y, -90.0f, 90.0f);
	transform.Rotation.Yaw = yaw;
	transform.Rotation.Pitch = pitch;
}

}