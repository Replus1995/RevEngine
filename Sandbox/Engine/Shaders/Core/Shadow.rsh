#ifndef _SHADOW_RSH_
#define _SHADOW_RSH_

#include "Engine/Shaders/Core/Samplers.rsh"

#define MAX_CASCADES 4

struct FShadowUniform
{
    float4x4 LightProjMats[6];
    float4x4 LightViewMats[6];   // For CSM: LightViewProj stored in [0..3]
    float4 LightParams;           // X = CascadeCount, YZW = CascadeSplits
};

// Shadow map texture array + comparison sampler (bound at t5/s5)
SHADOWMAP2DARRAY(sShadowMap, 5);

/**
 * Selects the cascade index based on the view-space depth of the world position.
 */
uint SelectCascade(float ViewSpaceDepth, float3 CascadeSplits, uint CascadeCount)
{
    uint CascadeIndex = CascadeCount - 1; // Default to last cascade
    for (uint i = 0; i < CascadeCount - 1; ++i)
    {
        if (ViewSpaceDepth < CascadeSplits[i])
        {
            CascadeIndex = i;
            break;
        }
    }
    return CascadeIndex;
}

/**
 * Performs hardware PCF sampling on a single shadow map cascade.
 */
float SampleShadowCascade(float4 LightSpacePos, uint CascadeIndex)
{
    float3 ProjCoords = LightSpacePos.xyz / LightSpacePos.w;
    float2 UV = ProjCoords.xy * 0.5 + 0.5;
    float Depth = ProjCoords.z;

    if (UV.x < 0.0 || UV.x > 1.0 || UV.y < 0.0 || UV.y > 1.0)
        return 1.0;

    // Hardware PCF via comparison sampler
    return sShadowMapTexture.SampleCmpLevelZero(sShadowMapSampler, float3(UV, float(CascadeIndex)), Depth);
}

/**
 * Computes the shadow factor for a world-space position using cascaded shadow maps.
 */
float ComputeShadowFactor(float3 WorldPos, float ViewSpaceDepth, FShadowUniform ShadowUniform)
{
    uint CascadeCount = uint(ShadowUniform.LightParams.x);
    float3 CascadeSplits = ShadowUniform.LightParams.yzw;

    uint CascadeIndex = SelectCascade(ViewSpaceDepth, CascadeSplits, CascadeCount);

    // Transform world pos to light clip space using the cascade's LightViewProj
    float4 LightSpacePos = mul(ShadowUniform.LightViewMats[CascadeIndex], float4(WorldPos, 1.0));

    return SampleShadowCascade(LightSpacePos, CascadeIndex);
}

#endif
