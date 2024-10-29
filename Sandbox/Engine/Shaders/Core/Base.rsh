#ifndef _BASE_RSH_
#define _BASE_RSH_

#define VS_SHADERMODEL vs_6_0
#define PS_SHADERMODEL ps_6_0
#define PI 3.1415926
#define EPSILON 0.00001

#define REGISTER(_type, _reg) register( _type[_reg] )

//Texture/Sampler defines
struct FSampler2D
{
	Texture2D Texture;
	SamplerState Sampler;
};
float4 Sample2D(in FSampler2D _sampler, in float2 _coord)
{
	return _sampler.Texture.Sample(_sampler.Sampler, _coord);
}

#define SAMPLER2D(_name, _reg) \
    uniform Texture2D _name ## Texture : REGISTER(t, _reg); \
    uniform SamplerState _name ## Sampler : REGISTER(s, _reg); \
    static FSampler2D _name = { _name ## Texture, _name ## Sampler }



//Default SamplerStates
/*
static SamplerState GTextureSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

static SamplerComparisonState GShadowSampler
{
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    AddressU = MIRROR;
    AddressV = MIRROR;
    ComparisonFunc = LESS;
};
*/

//Util Functions
float2x2 MatrixFromCols(in float2 Col0, in float2 Col1)
{
    return transpose(float2x2(Col0, Col1));
}

float3x3 MatrixFromCols(in float3 Col0, in float3 Col1, in float3 Col2)
{
    //return transpose(float3x3(Col0, Col1, Col2));
    return float3x3(Col0, Col1, Col2);
}

float4x4 MatrixFromCols(in float4 Col0, in float4 Col1, in float4 Col2, in float4 Col3)
{
    return transpose(float4x4(Col0, Col1, Col2, Col3));
}

float3x3 ToMatrix3(in float4x4 InMat)
{
    return MatrixFromCols(InMat[0], InMat[1], InMat[2]);
}


float4 DecodeColor(in uint InColor)
{
    float4 OutColor;
    OutColor.r = float(InColor >> 24) / 255.0f;
    OutColor.g = float((InColor & 0x00ff0000) >> 16) / 255.0f;
    OutColor.b = float((InColor & 0x0000ff00) >> 8) / 255.0f;
    OutColor.a = float(InColor & 0x000000ff) / 255.0f;

    return OutColor;
}

float3 ToLinear(in float3 InColor)
{
    return pow(abs(InColor), float3(2.2) );
}

float4 ToLinear(in float4 InColor)
{
    return float4(ToLinear(InColor.xyz), InColor.w);
}

float3 ToGamma(in float3 InColor)
{
	return pow(abs(InColor), float3(1.0/2.2) );
}

float4 ToGamma(in float4 InColor)
{
	return float4(ToGamma(InColor.xyz), InColor.w);
}

struct FRect2D
{
    uint X;
    uint Y;
    uint Width;
    uint Height;
};
struct FSceneUniform
{
    FRect2D ViewExtent;
    float4 ViewPos;
	float4x4 ViewMat;
    float4x4 ProjMat;
    float4x4 ViewProjMat;
    float4x4 InvViewProjMat;
};

struct FStaticMeshUniform
{
    float4x4 ModelMat;
};




#endif