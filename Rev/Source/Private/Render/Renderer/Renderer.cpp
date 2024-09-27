#include "Rev/Render/Renderer/Renderer.h"
#include "Rev/Core/Assert.h"

namespace Rev
{


FRenderer::FRenderer(uint32 InWidth, uint32 InHeight, FSceneProxy* InSceneProxy)
	: mSceneProxy(InSceneProxy)
{
	mSceneParams.ViewExtent = FRect2D{ 0, 0, InWidth, InHeight };
	mSceneUB = FRHICore::CreateUniformBuffer(sizeof(FSceneUniform), UL::BScene);
	mModelUB = FRHICore::CreateUniformBufferDynamic(REV_MODEL_UNIFROM_BUFFER_SIZE, UL::BModel);
}

FRenderer::~FRenderer()
{
	mSceneUB.reset();
	mModelUB.reset();
}

void FRenderer::BeginFrame()
{
	mSceneUB->UpdateSubData(&mSceneParams, sizeof(FSceneUniform));
}

void FRenderer::DrawFrame()
{
}

void FRenderer::EndFrame()
{
}

void FRenderer::SetScreenSize(uint32 InWidth, uint32 InHeight)
{
	if (InWidth > 0 && InWidth != mSceneParams.ViewExtent.Width)
	{
		mSceneParams.ViewExtent.Width = InWidth;
	}
	if (InHeight > 0 && InHeight != mSceneParams.ViewExtent.Height)
	{
		mSceneParams.ViewExtent.Height = InHeight;
	}
}

}