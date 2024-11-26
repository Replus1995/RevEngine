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

	virtual void BeginFrame(FRHICommandList& RHICmdList);
	virtual void DrawFrame(FRHICommandList& RHICmdList);
	virtual void EndFrame(FRHICommandList& RHICmdList);

private:
	void BuildFrameGraph();

private:
	FSceneProxy* mSceneProxy = nullptr;
	uint32 FrameWidth = 0;
	uint32 FrameHeight = 0;

	bool bDirtyFG = true;
	FFrameGraph mGraph;
};

}