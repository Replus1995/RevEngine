#pragma once
#include "Rev/World/System/System.h"
#include "Rev/Render/Camera.h"
#include "Rev/World/Entity.h"

namespace Rev
{
class CameraController;
class REV_API PlayerCameraSystem : public ISystem
{
	DECLARE_SYSTEM(PlayerCameraSystem)
public:
	virtual void OnInit() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnDestroy() override;

	void GetCameraMatrix(Math::FMatrix4& projMat, Math::FMatrix4& viewMat) const;
	bool SetPlayerCamera(Entity e);
	void SetPlayerCameraController(const Ref<CameraController>& c);
	
private:
	Entity mCamEntity;
	Ref<CameraController> mCamController;
};

}