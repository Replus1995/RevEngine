#ifndef _SHADOW_RSH_
#define _SHADOW_RSH_
#include "/Engine/Shaders/Core/Base.rsh"
#define REV_MAX_CSM_CASCADES 4
struct FDirectionalShadowUniform
{
    float4x4 LightViewProjMats[REV_MAX_CSM_CASCADES];
    float4 CascadeSplits;
    float4 Params;
};
#endif
