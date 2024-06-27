#include "pinepch.h"
#include "PlayerCameraSystem.h"
#include "Pine/Core/Application.h"
#include "Pine/Core/Window.h"
#include "Pine/World/Scene.h"
#include "Pine/World/Component/Component.h"
#include "Pine/World/System/Control/CameraController.h"
#include "Pine/World/System/Control/FreeCameraController.h"

namespace Pine
{

void PlayerCameraSystem::OnInit()
{
	if (!mCamController)
	{
		mCamController = CreateRef<FreeCameraController>();
	}
}

void PlayerCameraSystem::OnUpdate(float dt)
{
	if (mCamEntity)
	{
		auto [transformComp, cameraComp] = mRegistry.get<TransformComponent, CameraComponent>(mCamEntity);
		mCamController->OnUpdate(dt, cameraComp.Camera, transformComp.Transform);
	}
}

void PlayerCameraSystem::OnDestroy()
{
}

void PlayerCameraSystem::GetCameraMatrix(glm::mat4& projMat, glm::mat4& viewMat) const
{
	if (mCamEntity)
	{
		auto [transformComp, cameraComp] = mRegistry.get<TransformComponent, CameraComponent>(mCamEntity);
		if (cameraComp.AutoAspectRatio)
		{
			auto window = Application::GetApp().GetWindow();
			float asp = float(window->GetWidth()) / float(window->GetHeight());
			cameraComp.Camera.SetAspectRatio(asp);
		}
		projMat = cameraComp.Camera.GetProjectionMatrix();
		viewMat = glm::inverse(transformComp.GetMatrix());
	}
	else
	{
		auto window = Application::GetApp().GetWindow();
		float asp = float(window->GetWidth()) / float(window->GetHeight());
		projMat = glm::perspective(glm::radians(45.0f), asp, 0.01f, 1000.0f);
		viewMat = glm::mat4(1.0f);
	}
}

bool PlayerCameraSystem::SetPlayerCamera(Entity e)
{
	if (e.HasComponent<CameraComponent>())
	{
		mCamEntity = e;
		return true;
	}
	return false;
}

void PlayerCameraSystem::SetPlayerCameraController(const Ref<CameraController>& c)
{
	if(c != nullptr)
		mCamController = c;
}

}


