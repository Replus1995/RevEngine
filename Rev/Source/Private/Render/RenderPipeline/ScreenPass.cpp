#include "Rev/Render/RenderPipeline/ScreenPass.h"
#include "Rev/Render/RenderPipeline/RenderPipeline.h"
#include "Rev/Render/RHI/RHIResourceFactory.h"


namespace Rev
{

FScreenPass::FScreenPass(std::string_view InName, bool bOffScreen)
	: FRHIRenderPass(InName)
	, mOffScreen(bOffScreen)
{
}

FScreenPass::~FScreenPass()
{
}

void FScreenPass::BeginPass()
{
	PrepareTarget();
	if (mTarget)
	{
		mTarget->Bind();
	}
}

void FScreenPass::EndPass()
{
	if (mTarget)
	{
		mTarget->Unbind();
	}
}

void FScreenPass::PrepareTarget()
{
	if(!FRenderPipeline::GetCurrentPipeline())
		return;
	uint32 InWidth = FRenderPipeline::GetCurrentPipeline()->GetWidth();
	uint32 InHeight = FRenderPipeline::GetCurrentPipeline()->GetHeight();
	RE_CORE_ASSERT(InWidth > 0 && InHeight > 0);

	if (mOffScreen)
	{
		bool bNeedCreateTarget = true;
		if (mTarget)
		{
			const FRenderTargetDesc& CurDesc = mTarget->GetDesc();
			if (CurDesc.Width == InWidth && CurDesc.Height == InHeight)
			{
				bNeedCreateTarget = false;
			}
		}

		if (bNeedCreateTarget)
		{
			mTarget = FRHIResourceFactory::CreateRenderTarget(MakeTargetDesc(InWidth, InHeight));
		}
	}
}


}

