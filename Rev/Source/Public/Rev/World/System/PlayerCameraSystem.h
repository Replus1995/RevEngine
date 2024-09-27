#pragma once
#include "Rev/World/System/System.h"
#include "Rev/World/Entity.h"
#include "Rev/Render/Component/Camera.h"
#include "Rev/Render/UniformDefine.h"

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

	void FillCameraUniform(FSceneUniform& OutUniform) const;
	bool SetPlayerCamera(FEntity e);
	void SetPlayerCameraController(const Ref<CameraController>& c);
	
private:
	FEntity mCamEntity;
	Ref<CameraController> mCamController;
};

}