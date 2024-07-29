#include "Rev/Render/RenderPipeline/RenderPass.h"
#include "Rev/Render/RHI/RHIRenderTarget.h"

namespace Rev
{

FRenderPass::FRenderPass(std::string_view InName)
	: mName(InName)
	, mTarget(nullptr)
{
}

FRenderPass::~FRenderPass()
{
}

void FRenderPass::BeginPass()
{
	mInPass = true;
	if (mTarget)
	{
		mTarget->Bind();
	}
}

void FRenderPass::EndPass()
{
	if (mTarget)
	{
		mTarget->Unbind();
	}
	mInPass = false;
}

void FRenderPass::SetTarget(const Ref<FRHIRenderTarget>& InTarget)
{
	if (mInPass)
	{
		RE_CORE_ASSERT(false, "Never set render target in pass.")
		return;
	}
	mTarget = InTarget;
}

}