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
	uint32 LightViewCount = 1;
	Math::FVector3 LightParams;
};

struct FUnifiedLight
{
	uint32 Type; //ELightKind 
	Math::FVector3 Position;
	Math::FVector3 Direction;
	int32 ShadowIndex;
	Math::FVector3 Color;
	float Intensity;
	Math::FVector4 Params;
};

struct FForwardLightUniform
{
	FUnifiedLight Lights[UNIFORM_MAX_FORWARD_LIGHTS];
	uint32 LightCount = 0;
};

struct FDeferredLightUniform
{

};

}