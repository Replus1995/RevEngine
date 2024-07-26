#include "Rev/Render/RenderPipeline/PostPass.h"
#include "Rev/Render/RHI/RHIRenderTarget.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Core/Assert.h"

//Private
#include "Asset/BasicGeometry/PlaneGeometry.hpp"

namespace Rev
{

static Ref<FRHIVertexArray> sPostPassVertexData = nullptr;

FPostPass::FPostPass(std::string_view InName)
	: FRHIRenderPass(InName)
{
}

FPostPass::~FPostPass()
{
}

void FPostPass::BeginPass()
{
	mInPass = true;
	if (mTarget)
	{
		mTarget->Bind();
	}
}

void FPostPass::EndPass()
{
	if (mTarget)
	{
		mTarget->Unbind();
	}
	mInPass = false;
}

void FPostPass::SetRenderTarget(const Ref<FRHIRenderTarget>& InTarget)
{
	if (mInPass)
	{
		RE_CORE_ASSERT(false, "Never set render target in pass.")
		return;
	}
	mTarget = InTarget;
}

void FPostPass::DrawQuad()
{
	RenderCmd::DrawVertices(GetVertexData(), DM_Triangles);
}

void FPostPass::CreateVertexData()
{
	sPostPassVertexData = FPlaneGeometry::MakeVertexData();
}

void FPostPass::DestroyVertexData()
{
	sPostPassVertexData.reset();
}

const Ref<FRHIVertexArray>& FPostPass::GetVertexData()
{
	return sPostPassVertexData;
}

}


