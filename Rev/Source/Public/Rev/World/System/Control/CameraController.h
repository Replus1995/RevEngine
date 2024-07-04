#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Events/Event.h"
#include "Rev/Render/Camera.h"

namespace Rev
{

class CameraController
{
public:
	CameraController() = default;
	virtual ~CameraController() = default;

	virtual void OnUpdate(float dt, Camera& camera, Math::FTransform& transform) = 0;
	virtual void OnEvent(Event& e, Camera& camera, Math::FTransform& transform) = 0;
};

}