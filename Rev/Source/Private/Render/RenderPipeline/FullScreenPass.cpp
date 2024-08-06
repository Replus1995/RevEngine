#include "Rev/Render/RenderPipeline/FullScreenPass.h"
#include "Rev/Render/RHI/RHIRenderTarget.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Core/Assert.h"

//Private
#include "Asset/BasicGeometry/PlaneGeometry.hpp"

namespace Rev
{

static Ref<FRHIVertexArray> sPostPassVertexData = nullptr;

FFullScreenPass::FFullScreenPass(std::string_view InName)
	: FRenderPass(InName)
{
}

FFullScreenPass::~FFullScreenPass()
{
}

void FFullScreenPass::DrawQuad()
{
	RenderCmd::Draw(sPostPassVertexData, DM_Triangles);
}

void FFullScreenPass::CreatePrimitive()
{
	sPostPassVertexData = FPlaneGeometry::MakeVertexData();
}

void FFullScreenPass::DestroyPrimitive()
{
	sPostPassVertexData.reset();
}

}


