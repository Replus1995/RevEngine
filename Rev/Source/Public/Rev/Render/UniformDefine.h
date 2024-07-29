#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/RenderCore.h"

#define UNIFORM_MAX_FORWARD_LIGHTS 32

namespace Rev
{

struct FCameraUniform
{
	Math::FMatrix4 ProjMatrix;
	Math::FMatrix4 ViewMatrix;
	Math::FVector4 Position;
};

struct FModelUniform
{
	Math::FMatrix4 ModelMatrix;
};

struct FShadowUniform
{
	Math::FMatrix4 LightProjMats[6];
	Math::FMatrix4 LightViewMats[6];
	Math::FVector4 LightParams; //X = ViewCount
};

struct FUnifiedLight
{
	uint32 Type; //ELightKind 
	int32 ShadowIndex;
	float Intensity;
	alignas(16) Math::FVector4 Position;
	Math::FVector4 Direction;
	Math::FVector4 Color;
	Math::FVector4 Params;
};

struct FForwardLightUniform
{
	uint32 LightCount = 0;
	alignas(16) FUnifiedLight Lights[UNIFORM_MAX_FORWARD_LIGHTS];
};

struct FDeferredLightUniform
{

};

}