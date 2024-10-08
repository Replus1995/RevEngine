#include "Rev/World/System/PlayerCameraSystem.h"
#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"
#include "Rev/World/Scene.h"
#include "Rev/World/Component/AllComponents.h"
#include "Rev/World/System/Control/CameraController.h"
#include "./Control/FreeCameraController.h"

namespace Rev
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
		const auto& [transformComp, cameraComp] = mCamEntity.GetComponent<TransformComponent, CameraComponent>();
		mCamController->OnUpdate(dt, cameraComp.Camera, transformComp.Transform);
	}
}

void PlayerCameraSystem::OnDestroy()
{
}

void PlayerCameraSystem::FillCameraData(Math::FVector3& ViewPos, Math::FMatrix4& ViewMatrix, Math::FMatrix4& ProjMatrix) const
{
	if (mCamEntity)
	{
		const auto& [transformComp, cameraComp] = mCamEntity.GetComponent<TransformComponent, CameraComponent>();
		if (cameraComp.AutoAspectRatio)
		{
			auto window = Application::GetApp().GetWindow();
			float asp = float(window->GetWidth()) / float(window->GetHeight());
			cameraComp.Camera.SetAspectRatio(asp);
		}
		ProjMatrix = cameraComp.Camera.GetProjectionMatrix();
		ViewMatrix = transformComp.GetMatrix().Inverse();
		ViewPos = transformComp.Location();
	}
	else
	{
		auto window = Application::GetApp().GetWindow();
		float asp = float(window->GetWidth()) / float(window->GetHeight());
		ProjMatrix = Math::FMatrix4::Perspective(Math::Radians(45.0f), asp, 0.01f, 1000.0f);
		ViewMatrix = Math::FMatrix4(1.0f);
		ViewPos = Math::FVector3(0, 0, 0);
	}
}

bool PlayerCameraSystem::SetPlayerCamera(FEntity e)
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


