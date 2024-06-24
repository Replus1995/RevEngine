#include "pinepch.h"
#include "PlayerCameraSystem.h"
#include "Pine/World/Component/Component.h"
#include "Pine/Core/Application.h"
#include "Pine/Core/Window.h"

namespace Pine
{

void PlayerCameraSystem::GetCameraMatrix(glm::mat4& projMat, glm::mat4& viewMat) const
{
	auto view = mRegistry.view<TransformComponent, CameraComponent>();
	if (view.size_hint() > 0)
	{
		auto entity = *view.begin();
		auto [transformComp, cameraComp] = view.get<TransformComponent, CameraComponent>(entity);
		projMat = cameraComp.Camera.GetProjectionMatrix();
		viewMat = glm::inverse(transformComp.GetTransform());
	}
	else
	{
		auto window = Application::GetApp().GetWindow();
		float asp = float(window->GetWidth()) / float(window->GetHeight());
		projMat = glm::perspective(glm::radians(45.0f), asp, 0.01f, 1000.0f);
		viewMat = glm::mat4(1.0f);
	}
}
}


