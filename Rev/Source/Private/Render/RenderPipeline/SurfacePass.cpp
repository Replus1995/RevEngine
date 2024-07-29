#include "Rev/Render/RenderPipeline/SurfacePass.h"
#include "Rev/Render/RHI/RHIResourceFactory.h"

#include "Rev/Core/Assert.h"

namespace Rev
{

FSurfacePass::FSurfacePass()
	: FScreenPass("SurfacePass", true)
{
}

FSurfacePass::~FSurfacePass()
{
}

void FSurfacePass::BeginPass()
{
	FScreenPass::BeginPass();
}

void FSurfacePass::EndPass()
{
	FScreenPass::EndPass();
}

FRenderTargetDesc FSurfacePass::MakeTargetDesc(uint32 InWidth, uint32 InHeight)
{
	std::vector<FColorTargetDesc> vColorDesc;
	vColorDesc.push_back({ PF_R32G32B32A32F, Math::FLinearColor(0, 0, 0, 1) }); //BaseClor
	vColorDesc.push_back({ PF_R32G32B32A32F, Math::FLinearColor(0, 0, 0, 1) }); //Normal
	vColorDesc.push_back({ PF_R32G32B32A32F, Math::FLinearColor(0, 0, 0, 0) }); //MetallicRoughnessOcclusion
	vColorDesc.push_back({ PF_R32G32B32A32F, Math::FLinearColor(0, 0, 0, 1) }); //Emissive

	FRenderTargetDesc SurfaceDesc = FRenderTargetDesc::Make2D(InWidth, InHeight, vColorDesc.data(), vColorDesc.size(), { PF_DepthStencil, 1.0F, 0 });
	return SurfaceDesc;
}

}