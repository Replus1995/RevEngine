#include "Rev/Render/RenderPipeline/SurfacePass.h"
#include "Rev/Render/RenderProxy/SceneRenderProxy.h"
#include "Rev/Render/RHI/RHIResourceFactory.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FForwardSurfacePass::FForwardSurfacePass()
	: FRenderPass("ForwardSurfacePass")
{
}

FForwardSurfacePass::~FForwardSurfacePass()
{
}

void FForwardSurfacePass::RunPass(SceneRenderProxy& InSceneProxy)
{
	InSceneProxy.DrawScene();
}

FDeferredSurfacePass::FDeferredSurfacePass()
	: FRenderPass("SurfacePass")
{
}

FDeferredSurfacePass::~FDeferredSurfacePass()
{
}

FRenderTargetDesc MakeTargetDesc(uint32 InWidth, uint32 InHeight)
{
	std::vector<FColorTargetDesc> vColorDesc;
	vColorDesc.push_back({ PF_R32G32B32A32F, Math::FLinearColor(0, 0, 0, 1) }); //BaseColor
	vColorDesc.push_back({ PF_R32G32B32A32F, Math::FLinearColor(0, 0, 0, 1) }); //Normal
	vColorDesc.push_back({ PF_R32G32B32A32F, Math::FLinearColor(0, 0, 0, 0) }); //MetallicRoughnessOcclusion
	vColorDesc.push_back({ PF_R32G32B32A32F, Math::FLinearColor(0, 0, 0, 1) }); //Emissive

	FRenderTargetDesc SurfaceDesc = FRenderTargetDesc::Make2D(InWidth, InHeight, vColorDesc.data(), vColorDesc.size(), { PF_DepthStencil, 1.0F, 0 });
	return SurfaceDesc;
}

}