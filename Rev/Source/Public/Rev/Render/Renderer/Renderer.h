#pragma once
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/UniformDefine.h"
#include "Rev/Render/RHI/RHIBuffer.h"

namespace Rev
{
class FSceneProxy;
class FRenderer
{
public:
	FRenderer(uint32 InWidth, uint32 InHeight, FSceneProxy* InSceneProxy);
	virtual ~FRenderer();

	virtual void BeginFrame();
	virtual void DrawFrame();
	virtual void EndFrame();

	void SetScreenSize(uint32 InWidth, uint32 InHeight);
	const Ref<FRHIUniformBuffer>& GetBuiltInUB() const { return mBuiltInUB; }

protected:
	FScreenUniform mSreenParams;
	bool mScreenParamsDirty = true;
	FSceneProxy* mSceneProxy = nullptr;

	Ref<FRHIUniformBuffer> mBuiltInUB;

};

}