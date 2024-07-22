#include "Rev/Render/RenderPipeline/RenderPipeline.h"
#include "Rev/Render/RenderPipeline/PostPass.h"

namespace Rev
{

static FRenderPipeline* sCurrentPipeline = nullptr;

FRenderPipeline::FRenderPipeline()
	: mWidth(0)
	, mHeight(0)
{
}

FRenderPipeline::~FRenderPipeline()
{
}

void FRenderPipeline::BeginPipeline(uint32 InWidth, uint32 InHeight)
{
	mWidth = InWidth;
	mHeight = InHeight;
	sCurrentPipeline = this;
}

void FRenderPipeline::EndPipeline()
{
	sCurrentPipeline = nullptr;
}

const FRenderPipeline* FRenderPipeline::GetCurrentPipeline()
{
	return sCurrentPipeline;
}

void FRenderPipeline::Init()
{
	FPostPass::CreateVertexData(); 
}

void FRenderPipeline::Shutdown()
{
	FPostPass::DestroyVertexData();
}


}


