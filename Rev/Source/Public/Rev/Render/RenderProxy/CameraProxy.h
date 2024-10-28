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
	void SyncResource(FRHICommandList& RHICmdList);
	void FreeResource();

	const Math::FVector3& GetViewPos() const { return mViewPos; }
	const Math::FMatrix4& GetViewMat() const { return mViewMatrix; }
	const Math::FMatrix4& GetProjMat() const { return mProjMatrix; }
	Math::FMatrix4 GetViewProjMat() const { return mProjMatrix * mViewMatrix; }

private:
	Math::FVector3 mViewPos;
	Math::FMatrix4 mViewMatrix;
	Math::FMatrix4 mProjMatrix;
};

}