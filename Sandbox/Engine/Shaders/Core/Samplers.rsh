#ifndef _SAMPLER_RSH_
#define _SAMPLER_RSH_

#include "/Engine/Shaders/Core/Defines.rsh"

struct FSampler2D
{
	Texture2D Texture;
	SamplerState Sampler;

    float4 Sample(in float2 TexCoord)
    {
        return Texture.Sample(Sampler, TexCoord);
    }

    float4 SampleBias(in float2 TexCoord, in float Bias)
    {
        return Texture.SampleBias(Sampler, TexCoord, Bias);
    }
};

struct FSamplerCube
{
    TextureCube Texture;
	SamplerState Sampler;

    float4 Sample(in float3 TexCoord)
    {
        return Texture.Sample(Sampler, TexCoord);
    }

    float4 SampleBias(in float3 TexCoord, in float Bias)
    {
        return Texture.SampleBias(Sampler, TexCoord, Bias);
    }
};

#define SAMPLER2D(_name, _reg) \
    uniform Texture2D _name ## Texture : REGISTER(t, _reg); \
    uniform SamplerState _name ## Sampler : REGISTER(s, _reg); \
    static FSampler2D _name = { _name ## Texture, _name ## Sampler }

#define SAMPLERCUBE(_name, _reg) \
    uniform TextureCube _name ## Texture : REGISTER(t, _reg); \
    uniform SamplerState _name ## Sampler : REGISTER(s, _reg); \
    static FSamplerCube _name = { _name ## Texture, _name ## Sampler }


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

#endif