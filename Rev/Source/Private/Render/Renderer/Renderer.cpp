#include "Rev/Render/Renderer/Renderer.h"
#include "Rev/Core/Assert.h"

namespace Rev
{


FRenderer::FRenderer(uint32 InWidth, uint32 InHeight, FSceneProxy* InSceneProxy)
	: mSreenParams(InWidth, InHeight)
	, mSceneProxy(InSceneProxy)
{
	mBuiltInUB = FRHICore::CreateUniformBuffer(REV_BUILTIN_UNIFORM_SIZE);
}

FRenderer::~FRenderer()
{
	mBuiltInUB.reset();
}

void FRenderer::BeginFrame()
{
	if (mScreenParamsDirty)
	{
		mScreenParamsDirty = false;
		mBuiltInUB->UpdateSubData(&mSreenParams, sizeof(FScreenUniform), REV_SCREEN_UNIFORM_OFFSET);
	}
}

void FRenderer::DrawFrame()
{
}

void FRenderer::EndFrame()
{
}

void FRenderer::SetScreenSize(uint32 InWidth, uint32 InHeight)
{
	if (InWidth > 0 && InWidth != mSreenParams.Width)
	{
		mSreenParams.Width = InWidth;
		mScreenParamsDirty = true;
	}
	if (InHeight > 0 && InHeight != mSreenParams.Height)
	{
		mSreenParams.Height = InHeight;
		mScreenParamsDirty = true;
	}
}

}