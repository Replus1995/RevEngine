#include "Rev/Render/RenderPipeline/RenderPass.h"

namespace Rev
{

FRHIRenderPass::FRHIRenderPass(std::string_view InName)
	: mName(InName)
	, mTarget(nullptr)
{
}

FRHIRenderPass::~FRHIRenderPass()
{
}

}