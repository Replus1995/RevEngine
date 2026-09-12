#ifndef _BASE_RSH_
#define _BASE_RSH_

#include "/Engine/Shaders/Core/Defines.rsh"
#include "/Engine/Shaders/Core/Samplers.rsh"
#include "/Engine/Shaders/Core/Utils.rsh"

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
    float4x4 InvViewMat;
    float4x4 InvProjMat;
    float4x4 InvViewProjMat;
};

struct FStaticMeshUniform
{
    float4x4 ModelMat;
};

#endif