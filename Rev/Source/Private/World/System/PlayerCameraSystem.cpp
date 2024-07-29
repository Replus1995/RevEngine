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
		auto [transformComp, cameraComp] = mRegistry.get<TransformComponent, CameraComponent>(mCamEntity);
		mCamController->OnUpdate(dt, cameraComp.Camera, transformComp.Transform);
	}
}

void PlayerCameraSystem::OnDestroy()
{
}

void PlayerCameraSystem::FillCameraUniform(FCameraUniform& OutUniform) const
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
		OutUniform.ProjMatrix = cameraComp.Camera.GetProjectionMatrix();
		OutUniform.ViewMatrix = transformComp.GetMatrix().Inverse();
		OutUniform.Position = Math::FVector4(transformComp.Location(), 1.0f);
	}
	else
	{
		auto window = Application::GetApp().GetWindow();
		float asp = float(window->GetWidth()) / float(window->GetHeight());
		OutUniform.ProjMatrix = Math::FMatrix4::Perspective(Math::Radians(45.0f), asp, 0.01f, 1000.0f);
		OutUniform.ViewMatrix = Math::FMatrix4(1.0f);
		OutUniform.Position = Math::FVector4(0, 0, 0, 1);
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


