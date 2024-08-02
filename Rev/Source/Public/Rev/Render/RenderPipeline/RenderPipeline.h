#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{
class SceneRenderProxy;
class FRenderPass;
class FRenderPipeline
{
public:
	FRenderPipeline();
	virtual ~FRenderPipeline();
	virtual void BeginPipeline(uint32 InWidth, uint32 InHeight, SceneRenderProxy* InSceneProxy);
	virtual void RunPipeline();
	virtual void EndPipeline();

	uint32 GetWidth() const { return mWidth; }
	uint32 GetHeight() const { return mHeight; }
	SceneRenderProxy* GetSceneProxy() const { return mSceneProxy; }

	static const FRenderPipeline* GetCurrentPipeline();
	static void Init();
	static void Shutdown();
protected:
	void RunPass(FRenderPass* Pass);

protected:
	uint32 mWidth;
	uint32 mHeight;
	SceneRenderProxy* mSceneProxy;
};

}