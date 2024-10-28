#include "Rev/Render/RenderPass/RenderPass.h"

namespace Rev
{

FRenderPass::FRenderPass(std::string_view InName)
	: mName(InName)
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

}