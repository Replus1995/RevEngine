#include "Rev/World/System/PlayerCameraSystem.h"
#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"
#include "Rev/World/Scene.h"
#include "Rev/World/Component/AllComponents.h"
#include "Rev/World/System/Control/CameraController.h"
#include "./Control/FreeCameraController.h"

namespace Rev
{
namespace PlayerCameraSystemPrivate
{
float GetFramebufferAspectRatio()
{
	Window* FrameWindow = Application::GetApp().GetWindow();
	int32 FrameWidth = 0;
	int32 FrameHeight = 0;
	FrameWindow->GetFrameSize(FrameWidth, FrameHeight);
	return FrameHeight > 0 ? float(FrameWidth) / float(FrameHeight) : 1.0f;
}
}

using namespace PlayerCameraSystemPrivate;

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
		const auto& [transformComp, cameraComp] = mCamEntity.GetComponents<TransformComponent, CameraComponent>();
		mCamController->OnUpdate(dt, cameraComp.Camera, transformComp.Transform);
	}
}

void PlayerCameraSystem::OnDestroy()
{
}

void PlayerCameraSystem::FillCameraData(Math::FVector3& ViewPos, Math::FMatrix4& ViewMatrix, Math::FMatrix4& ProjMatrix, Camera::FCameraProjectionInfo& ProjectionInfo)
{
	if (mCamEntity)
	{
		auto Comps = mCamEntity.GetComponents<TransformComponent, CameraComponent>();
		auto& [transformComp, cameraComp] = Comps;
		if (cameraComp.AutoAspectRatio)
		{
			cameraComp.Camera.SetAspectRatio(GetFramebufferAspectRatio());
		}
		ProjMatrix = cameraComp.Camera.GetProjectionMatrix();
		ProjectionInfo = cameraComp.Camera.GetProjectionInfo();
		ViewMatrix = transformComp.GetMatrix().Inverse();
		ViewPos = transformComp.Location();
	}
	else
	{
		const float asp = GetFramebufferAspectRatio();
		ProjMatrix = Math::FMatrix4::Perspective(Math::Radians(45.0f), asp, 0.01f, 1000.0f);
		ProjectionInfo = {};
		ProjectionInfo.AspectRatio = asp;
		ProjectionInfo.FarClip = 1000.0f;
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


