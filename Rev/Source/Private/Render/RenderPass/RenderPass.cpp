#include "Rev/Render/RenderPass/RenderPass.h"
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
	//RenderCmd::BindRenderTarget(mRenderTarget);
}

void FRenderPass::EndPass()
{
	//RenderCmd::BindRenderTarget(nullptr);
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
		REV_CORE_ASSERT(false, "Never set render target in pass.")
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