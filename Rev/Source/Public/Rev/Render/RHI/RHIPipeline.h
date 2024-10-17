#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/RHI/RHIDefinitions.h"

namespace Rev
{

//TODO: Tessellation State
//TODO: MultiSampling State
//TODO: Stencil Compare State

struct FColorBlendState
{
public:
	bool bEnable = false;
	EBlendFactor SrcColorFactor = BF_One;
	EBlendFactor DstColorFactor = BF_Zero;
	EBlendOperation ColorOp = BO_Add;
	EBlendFactor SrcAlphaBlendFactor = BF_One;
	EBlendFactor DstAlphaBlendFactor = BF_Zero;
	EBlendOperation AlphaBlendOp = BO_Add;
	EColorWriteMask ColorWriteMask = CWM_RGBA;
};

struct FRHIGraphicsState
{
public:
	//InputAssembly
	EPrimitiveTopology PrimitiveTopology = PT_Unknown;
	//Rasterization
	ECullMode CullMode = CM_None;
	EPolygonMode PolygonMode = PM_Fill;
	bool DepthClampEnable = false;
	bool DepthBiasEnable = false;
	float DepthBiasConstantFactor;
	float DepthBiasClamp;
	float DepthBiasSlopeFactor;
	float LineWidth;
	//DepthStencil
	bool DepthTestEnable = false;
	bool DepthWriteEnable = false;
	ECompareOperation DepthCompareOp = CO_Never;
	//ColorBlend
	FColorBlendState ColorBlendStates[RTA_MaxColorAttachments];
	Math::FLinearColor BlendConstants;
};




}