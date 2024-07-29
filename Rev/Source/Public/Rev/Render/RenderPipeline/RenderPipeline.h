#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{
class SceneRenderProxy;
class FRenderPipeline
{
public:
	FRenderPipeline();
	virtual ~FRenderPipeline();
	virtual void BeginPipeline(uint32 InWidth, uint32 InHeight);
	virtual void RunPipeline(SceneRenderProxy& InSceneProxy);
	virtual void EndPipeline();

	uint32 GetWidth() const { return mWidth; }
	uint32 GetHeight() const { return mHeight; }
	static const FRenderPipeline* GetCurrentPipeline();
	static void Init();
	static void Shutdown();
protected:
	uint32 mWidth;
	uint32 mHeight;

};

}