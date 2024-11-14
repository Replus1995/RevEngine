#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RHI/RHIDefinitions.h"
#include "Rev/Render/RHI/RHIState.h"
#include <unordered_map>

namespace Rev
{

//TODO: MultiSampling State

struct FRHIGraphicsPipelineStateDesc
{
public:
	EPrimitiveTopology PrimitiveTopology = PT_Triangles;

	FRHIRasterizerState* RasterizerState = nullptr;
	FRHIDepthStencilState* DepthStencilState = nullptr;
	FRHIColorBlendState* ColorBlendState = nullptr;
	FRHIVertexInputState* VertexInputState = nullptr;
	uint8 NumSamples = 1;

	friend bool operator==(const FRHIGraphicsPipelineStateDesc& A, const FRHIGraphicsPipelineStateDesc& B);
};

class FRHIPipelineStateCache
{
public:
	static void Initialize(ERenderAPI InAPI);
	static void Shutdown();
	static inline FRHIPipelineStateCache* Get() { return PipelineStateCache; }

	FRHIRasterizerState* GetOrCreateRasterizerState(const FRHIRasterizerStateDesc& InDesc);
	FRHIDepthStencilState* GetOrCreateDepthStencilState(const FRHIDepthStencilStateDesc& InDesc);
	FRHIColorBlendState* GetOrCreateColorBlendState(const FRHIColorBlendStateDesc& InDesc);

private:
	FRHIPipelineStateCache() = default;
	~FRHIPipelineStateCache() = default;

private:
	static FRHIPipelineStateCache* PipelineStateCache;
	std::unordered_map<FRHIRasterizerStateDesc, Ref<FRHIRasterizerState>> RasterizerStateCache;
	std::unordered_map<FRHIDepthStencilStateDesc, Ref<FRHIDepthStencilState>> DepthStencilStateCache;
	std::unordered_map<FRHIColorBlendStateDesc, Ref<FRHIColorBlendState>> ColorBlendStatCache;
};


}