#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/UniformDefine.h"
#include "Rev/Render/Component/Camera.h"

namespace Rev
{
class FRenderer;
class FScene;
class FCameraProxy
{
public:
	FCameraProxy();
	~FCameraProxy();

	void Prepare(const Ref<FScene>& Scene);
	void SyncResource(const FRenderer* Renderer);
	void FreeResource();

	const Math::FMatrix4& GetViewMat() const { return mCameraParams.ViewMat; }
	const Math::FMatrix4& GetProjMat() const { return mCameraParams.ProjMat; }
	const Math::FMatrix4& GetViewProjMat() const { return mCameraParams.ViewProjMat; }
	const Math::FMatrix4& GetInvViewProjMat() const { return mCameraParams.InvViewProjMat; }

private:
	FSceneUniform mCameraParams;
};

}