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
	FSceneUniform& GetSceneParams() { return mSceneParams; }
	const Ref<FRHIUniformBuffer>& GetSceneUB() const { return mSceneUB; }
	const Ref<FRHIUniformBufferDynamic>& GetModelUB() const { return mModelUB; }

protected:
	FSceneProxy* mSceneProxy = nullptr;
	FSceneUniform mSceneParams;
	Ref<FRHIUniformBuffer> mSceneUB;
	Ref<FRHIUniformBufferDynamic> mModelUB;

};

}