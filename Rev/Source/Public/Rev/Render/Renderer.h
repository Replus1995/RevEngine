#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderProxy/SceneProxy.h"
#include "Rev/Render/FrameGraph.h"

namespace Rev
{
class FRHITexture;
class FRHIRenderPass;
class FRenderer 
{
public:
	FRenderer(FSceneProxy* InSceneProxy);
	virtual ~FRenderer();

	virtual void BeginFrame();
	virtual void DrawFrame(FRHICommandList& RHICmdList);
	virtual void EndFrame();

private:
	void BuildFrameGraph();

private:
	FSceneProxy* mSceneProxy = nullptr;
	uint32 FrameWidth = 0;
	uint32 FrameHeight = 0;
	//Ref<FRHIRenderPass> mBasePass = nullptr;
	//Ref<FRHITexture> mBaseColorTex = nullptr;
	//Ref<FRHITexture> mBaseColorResolveTex = nullptr;
	//Ref<FRHITexture> mBaseDepthTex = nullptr;
	//Ref<FRHITexture> mBaseDepthResolveTex = nullptr;
	//Ref<FRHIRenderPass> mSkyPass = nullptr;

	Scope<FrameGraph> mFG;
	Scope<FrameGraphBlackboard> mFGB;
	bool bDirtyFG = true;
};

}