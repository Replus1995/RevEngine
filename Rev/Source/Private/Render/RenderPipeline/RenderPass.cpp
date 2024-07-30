#include "Rev/Render/RenderPipeline/RenderPass.h"
#include "Rev/Render/RHI/RHIRenderTarget.h"
#include "Rev/Render/RenderCmd.h"

namespace Rev
{

FRenderPass::FRenderPass(std::string_view InName)
	: mName(InName)
	, mRenderTarget(nullptr)
{
}

FRenderPass::~FRenderPass()
{
}

void FRenderPass::BeginPass()
{
	mInPass = true;
	if (mRenderTarget)
	{
		mRenderTarget->Bind();
	}
}

void FRenderPass::EndPass()
{
	if (mRenderTarget)
	{
		mRenderTarget->Unbind();
	}
	mInPass = false;
}

const Ref<FRHIRenderTarget>& FRenderPass::GetRenderTarget() const
{
	return mRenderTarget;
}

void FRenderPass::SetRenderTarget(const Ref<FRHIRenderTarget>& InTarget)
{
	if (mInPass)
	{
		RE_CORE_ASSERT(false, "Never set render target in pass.")
		return;
	}
	mRenderTarget = InTarget;
}

void FRenderPass::ClearRenderTarget()
{
	if (mRenderTarget)
	{
		mRenderTarget->ClearTargets();
	}
	else
	{
		RenderCmd::ClearBackBuffer();
	}
}

}