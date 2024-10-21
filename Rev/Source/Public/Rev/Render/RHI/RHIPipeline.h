#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/RHI/RHIDefinitions.h"
#include "Rev/Render/RHI/RHIState.h"

namespace Rev
{

//TODO: MultiSampling State

struct FRHIGraphicsPipelineStateDesc
{
public:
	EPrimitiveTopology PrimitiveTopology;

	FRHIRasterizerStateDesc RasterizerStateDesc;
	FRHIDepthStencilStateDesc DepthStencilStateDesc;
	FRHIColorBlendStateDesc ColorBlendStateDesc;
};




}