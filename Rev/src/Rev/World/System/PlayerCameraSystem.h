#pragma once
#include "Pine/World/System/System.h"
#include "Pine/Render/Camera.h"
#include "Pine/World/Entity.h"

namespace Pine
{
class CameraController;
class PINE_API PlayerCameraSystem : public ISystem
{
	DECLARE_SYSTEM(PlayerCameraSystem)
public:
	virtual void OnInit() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnDestroy() override;

	void GetCameraMatrix(FMatrix4& projMat, FMatrix4& viewMat) const;
	bool SetPlayerCamera(Entity e);
	void SetPlayerCameraController(const Ref<CameraController>& c);
	
private:
	Entity mCamEntity;
	Ref<CameraController> mCamController;
};

}