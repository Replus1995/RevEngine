#include "Rev/Render/RenderPipeline/RenderPipeline.h"
#include "Rev/Render/RenderPipeline/RenderPass.h"
#include "Rev/Render/RenderPipeline/FullScreenPass.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

static FRenderPipeline* sCurrentPipeline = nullptr;

FRenderPipeline::FRenderPipeline()
	: mWidth(0)
	, mHeight(0)
	, mSceneProxy(nullptr)
{
}

FRenderPipeline::~FRenderPipeline()
{
}

void FRenderPipeline::BeginPipeline(uint32 InWidth, uint32 InHeight, SceneRenderProxy* InSceneProxy)
{
	RE_CORE_ASSERT(InSceneProxy != nullptr);

	mWidth = InWidth;
	mHeight = InHeight;
	mSceneProxy = InSceneProxy;
	sCurrentPipeline = this;
}

void FRenderPipeline::RunPipeline()
{
}

void FRenderPipeline::EndPipeline()
{
	sCurrentPipeline = nullptr;
	mSceneProxy = nullptr;
}

const FRenderPipeline* FRenderPipeline::GetCurrentPipeline()
{
	return sCurrentPipeline;
}

void FRenderPipeline::Init()
{
	FFullScreenPass::CreatePrimitive();
}

void FRenderPipeline::Shutdown()
{
	FFullScreenPass::DestroyPrimitive();
}

void FRenderPipeline::RunPass(FRenderPass* Pass)
{
	RE_CORE_ASSERT(Pass != nullptr);
	Pass->BeginPass();
	Pass->RunPass();
	Pass->EndPass();
}


}


