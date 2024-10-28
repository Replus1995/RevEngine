#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/UniformDefine.h"
#include "Rev/Render/Component/Light.h"

namespace Rev
{
class FRHIUniformBuffer;
class FScene;
class FLightProxy
{
public:
	FLightProxy();
	~FLightProxy();

	void Prepare(const Ref<FScene>& Scene);
	void SyncResource(FRHICommandList& RHICmdList);
	void FreeResource();

private:
	FDirectionalLightUniform mDirectionalLightParams;
	Ref<FRHIUniformBuffer> mLightUB;
};

}