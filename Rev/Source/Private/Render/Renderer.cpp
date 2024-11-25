#include "Rev/Render/Renderer.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RHI/RHIPipeline.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Render/RenderUtils.h"
#include "Rev/Render/RenderOptions.h"
#include <fstream>

#include "FrameGraph/BaseRendering.h"
#include "FrameGraph/SkyRendering.h"


namespace Rev
{

FRenderer::FRenderer(FSceneProxy* InSceneProxy)
	: mSceneProxy(InSceneProxy)
{
}

FRenderer::~FRenderer()
{
}

void FRenderer::BeginFrame(FRHICommandList& RHICmdList)
{
	//prepare resource

	uint32 NewFrameWidth = RHICmdList.GetContext()->RHIGetFrameWidth();
	uint32 NewFrameHeight = RHICmdList.GetContext()->RHIGetFrameHeight();

	if (FrameWidth != NewFrameWidth || FrameHeight != NewFrameHeight)
	{
		FrameWidth = NewFrameWidth;
		FrameHeight = NewFrameHeight;

		bDirtyFG = true;
	}

	if (bDirtyFG)
	{
		BuildFrameGraph();
		bDirtyFG = false;
	}

}

void FRenderer::DrawFrame(FRHICommandList& RHICmdList)
{
	mSceneProxy->DrawShadowMaps(RHICmdList);
	mSceneProxy->SyncResource(RHICmdList);

	FFGExecuteData ContextData(RHICmdList, mSceneProxy);
	mGraph.Execute(ContextData);
}

void FRenderer::EndFrame(FRHICommandList& RHICmdList)
{
}

void FRenderer::BuildFrameGraph()
{

	mGraph.Reset();

	FFGSetupData ViewData = { FrameWidth,  FrameHeight };
	FFGBasePass BasePass(mGraph, ViewData);
	FFGSkyPass SkyPass(mGraph, ViewData);
	FFGBlitPass BlitPass(mGraph, ViewData);

	mGraph.Compile();

	std::ofstream{ "FrameGraph.dot" } << mGraph;
}

}