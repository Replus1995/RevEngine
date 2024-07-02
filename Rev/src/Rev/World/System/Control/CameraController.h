#pragma once
#include "Pine/Core/Base.h"
#include "Pine/Events/Event.h"
#include "Pine/Render/Camera.h"
#include "Pine/Math/Transform.h"

namespace Pine
{

class CameraController
{
public:
	CameraController() = default;
	virtual ~CameraController() = default;

	virtual void OnUpdate(float dt, Camera& camera, FTransform& transform) = 0;
	virtual void OnEvent(Event& e, Camera& camera, FTransform& transform) = 0;
};

}