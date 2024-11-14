#include "Rev/Render/RHI/RHIPipeline.h"
#include "Rev/Render/RHI/DynamicRHI.h"

namespace Rev
{

bool operator==(const FRHIGraphicsPipelineStateDesc& A, const FRHIGraphicsPipelineStateDesc& B)
{
	bool bSame = A.PrimitiveTopology == B.PrimitiveTopology &&
		A.RasterizerState == B.RasterizerState &&
		A.DepthStencilState == B.DepthStencilState &&
		A.ColorBlendState == B.ColorBlendState &&
		A.VertexInputState == B.VertexInputState;
	return bSame;
}

FRHIPipelineStateCache* FRHIPipelineStateCache::PipelineStateCache = nullptr;

void FRHIPipelineStateCache::Initialize(ERenderAPI InAPI)
{
	if (PipelineStateCache == nullptr)
	{
		PipelineStateCache = new FRHIPipelineStateCache;
	}
}

void FRHIPipelineStateCache::Shutdown()
{
	if (PipelineStateCache != nullptr)
	{
		SAFE_DELETE(PipelineStateCache);
	}
}

FRHIRasterizerState* FRHIPipelineStateCache::GetOrCreateRasterizerState(const FRHIRasterizerStateDesc& InDesc)
{
	if (auto Iter = RasterizerStateCache.find(InDesc); Iter != RasterizerStateCache.end())
	{
		return Iter->second.get();
	}

	Ref<FRHIRasterizerState> NewState = GDynamicRHI->RHICreateRasterizerState(InDesc);
	RasterizerStateCache.emplace(InDesc, NewState);
	return NewState.get();
}

FRHIDepthStencilState* FRHIPipelineStateCache::GetOrCreateDepthStencilState(const FRHIDepthStencilStateDesc& InDesc)
{
	if (auto Iter = DepthStencilStateCache.find(InDesc); Iter != DepthStencilStateCache.end())
	{
		return Iter->second.get();
	}

	Ref<FRHIDepthStencilState> NewState = GDynamicRHI->RHICreateDepthStencilStateState(InDesc);
	DepthStencilStateCache.emplace(InDesc, NewState);
	return NewState.get();
}

FRHIColorBlendState* FRHIPipelineStateCache::GetOrCreateColorBlendState(const FRHIColorBlendStateDesc& InDesc)
{
	if (auto Iter = ColorBlendStatCache.find(InDesc); Iter != ColorBlendStatCache.end())
	{
		return Iter->second.get();
	}

	Ref<FRHIColorBlendState> NewState = GDynamicRHI->RHICreateColorBlendState(InDesc);
	ColorBlendStatCache.emplace(InDesc, NewState);
	return NewState.get();
}

}