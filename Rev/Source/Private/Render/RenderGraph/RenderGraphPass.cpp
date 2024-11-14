#include "Rev/Render/RenderGraph/RenderGraphPass.h"
#include "Rev/Render/RenderGraph/RenderGraphResources.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIRenderPass.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHICommandList.h"

namespace Rev
{

FRGPass::FRGPass(FRGName&& InName, FRGParameterStruct InParameterStruct, ERGPassFlags InFlags)
	: Name(InName)
	, ParameterStruct(InParameterStruct)
	, Flags(InFlags)
	, Pipeline(ERHIPipeline::Graphics)
{
}

const char* FRGPass::GetName() const
{
	return Name.GetCharName();
}

void FRGPass::InitRHI(const FRGRenderTargetBindings* RenderTargets)
{
	FRHIRenderPassDesc PassDesc;
	if (RenderTargets != nullptr)
	{
		uint32 ColorCount = RenderTargets->GetNumActive();
		for (uint32 i = 0; i < ColorCount; i++)
		{
			const FRGColorTargetBinding& ColorBinding = RenderTargets->Colors[i];
			PassDesc.ColorRenderTargets[i] = { ColorBinding.Texture->GetRHI(), ColorBinding.ResolveTexture->GetRHI(), ColorBinding.ArraySlice, ColorBinding.MipIndex, ColorBinding.LoadAction, RTS_Store};
		}
		PassDesc.NumColorRenderTargets = ColorCount;
		if (RenderTargets->DepthStencil.IsValid())
		{
			const FRGDepthStencilTargetBinding& DepthStencilBinding = RenderTargets->DepthStencil;
			PassDesc.DepthStencilRenderTarget = { DepthStencilBinding.Texture->GetRHI(), DepthStencilBinding.ResolveTexture->GetRHI(), DepthStencilBinding.DepthLoadAction, RTS_Store, DepthStencilBinding.StencilLoadAction, RTS_Store};
		}
	}
	RenderPassRHI = GDynamicRHI->RHICreateRenderPass(PassDesc);
}

bool FRGPass::IsReadyForDraw() const
{
	return RenderPassRHI != nullptr;
}

void FRGPass::BeginPass(FRHICommandList& RHICmdList)
{
	RHICmdList.GetContext()->RHIBeginDebugLabel(GetName(), Math::FLinearColor(0.8f, 1.0f, 0.8f));
	RHICmdList.GetContext()->RHIBeginRenderPass(RenderPassRHI.get());
}

void FRGPass::EndPass(FRHICommandList& RHICmdList)
{
	RHICmdList.GetContext()->RHIEndRenderPass();
	RHICmdList.GetContext()->RHIEndDebugLabel();
}


}

