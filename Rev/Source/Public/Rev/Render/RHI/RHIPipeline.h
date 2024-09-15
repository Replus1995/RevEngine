#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RHI/RHIPrimitive.h"
#include <vector>

namespace Rev
{

//TODO: Tessellation State
//TODO: MultiSampling State
//TODO: Stencil Compare State

enum EPolygonMode : uint8
{
	PM_Fill = 0,
	PM_Line = 1,
	PM_Point = 2
};

enum ECullMode : uint8
{
	CM_None = 0,
	CM_Back,
	CM_Front,
	CM_BackAndFront
};

enum ECompareOp : uint8
{
	CO_Never = 0,
	CO_Less,
	CO_Equal,
	CO_Greater,
	CO_NotEqual,
	CO_LessEqual,
	CO_GreaterEqual,
	CO_Always
};

enum EBlendFactor : uint8
{
	BF_Zero = 0,
	BF_One = 1,
	BF_SrcColor = 2,
	BF_OneMinusSrcColor = 3,
	BF_DstColor = 4,
	BF_OneMinusDstColor = 5,
	BF_SrcAlpha = 6,
	BF_OneMinusSrcAlpha = 7,
	BF_DstAlpha = 8,
	BF_OneMinusDstAlpha = 9,
	BF_ConstantColor = 10,
	BF_OneMinusConstantColor = 11,
	BF_ConstantAlpha = 12,
	BF_OneMinusConstantAlpha = 13,
};

enum EBlendOp : uint8
{
	BO_Add = 0,
	BO_Subtract = 1,
	BO_ReverseSubtract = 2,
	BO_Min = 3,
	BO_Max = 4,
};

enum EColorComponentMask : uint8
{
	CCM_Red = 0x00000001,
	CCM_Green = 0x00000002,
	CCM_Blue = 0x00000004,
	CCM_Alpha = 0x00000008,
	CCM_RGB = CCM_Red | CCM_Green | CCM_Blue,
	CCM_RGBA = CCM_RGB | CCM_Alpha
};

struct FColorBlendAttachmentState
{
public:
	bool BlendEnable = false;
	EBlendFactor SrcColorBlendFactor = BF_One;
	EBlendFactor DstColorBlendFactor = BF_Zero;
	EBlendOp ColorBlendOp = BO_Add;
	EBlendFactor SrcAlphaBlendFactor = BF_One;
	EBlendFactor DstAlphaBlendFactor = BF_Zero;
	EBlendOp AlphaBlendOp = BO_Add;
	EColorComponentMask ColorWriteMask = CCM_RGBA;
};

struct FRHIGraphicsPipelineState
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
	ECompareOp DepthCompareOp = CO_Never;
	//ColorBlend
	FColorBlendAttachmentState ColorBlendAttachmentState;
	Math::FLinearColor BlendConstants;
};



enum EPipelineBindPoint
{
	PBP_Graphics,
	PBP_Compute,
	PBP_RayTracing
};


}