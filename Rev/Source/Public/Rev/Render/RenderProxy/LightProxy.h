#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/UniformDefine.h"
#include "Rev/Render/Component/Light.h"

namespace Rev
{
class FRHICommandList;
class FRHIUniformBuffer;
class FScene;
class FLightProxy
{
public:
	FLightProxy();
	~FLightProxy();

	void Prepare(const Ref<FScene>& Scene);
	void SyncResource(FRHICommandList& RHICmdList);
	bool HasShadowCastingDirectionalLight() const { return mHasShadowCastingDirectionalLight; }
	const Math::FVector3& GetShadowDirection() const { return mShadowDirection; }
	void SetShadowCascadeCount(uint32 InCount);

private:
	FDirectionalLightUniform mDirectionalLightParams;
	Ref<FRHIUniformBuffer> mLightUB;
	bool mHasShadowCastingDirectionalLight = false;
	bool mWarnedAboutMultipleShadowLights = false;
	Math::FVector3 mShadowDirection = Math::FVector3(0.0f, -1.0f, 0.0f);
};

}
