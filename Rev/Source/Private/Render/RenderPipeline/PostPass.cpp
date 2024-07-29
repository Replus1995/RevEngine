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
	: FRenderPass(InName)
{
}

FPostPass::~FPostPass()
{
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


