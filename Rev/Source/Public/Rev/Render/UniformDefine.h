#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Utils/Rect2D.h"
#include "Rev/Render/RenderCore.h"

namespace Rev
{


struct alignas(16) FSceneUniform
{
	FRect2D ViewExtent;
	Math::FVector4 ViewPos;
	Math::FMatrix4 ViewMat;
	Math::FMatrix4 ProjMat;
	Math::FMatrix4 ViewProjMat;
	Math::FMatrix4 InvViewMat;
	Math::FMatrix4 InvProjMat;
	Math::FMatrix4 InvViewProjMat;
};

struct alignas(16) FStaticMeshUniform
{
	Math::FMatrix4 ModelMat;
};

constexpr uint32 REV_MAX_CSM_CASCADES = 4;

struct alignas(16) FDirectionalShadowUniform
{
	Math::FMatrix4 LightViewProjMats[REV_MAX_CSM_CASCADES];
	Math::FVector4 CascadeSplits;
	Math::FVector4 Params; // X = cascade count, Y = inverse resolution, Z = transition fraction, W = normal bias
};

struct alignas(16) FShadowPassUniform
{
	Math::FMatrix4 LightViewProjMat;
};

struct FDirectionalLightUniform
{
	struct alignas(16) Info {
		Math::FVector4 Direction;
		Math::FVector4 Color;
		float Intensity;
		int32 ShadowIndex;
		uint32 ShadowCount;
	}  Lights[REV_MAX_DIRECTIONAL_LIGHTS];
	uint32 Count = 0;
};

struct FSpotLightUniform
{
	uint32 Count = 0;
	struct alignas(16) Info {
		Math::FVector4 Position;
		Math::FVector4 Direction;
		Math::FVector4 Color;
		float Intensity;
		int32 ShadowIndex;
		float Radius;
		float Angle;
	} Lights[REV_MAX_SPOT_LIGHTS];
};

struct FPointLightUniform
{
	uint32 Count = 0;
	struct alignas(16) Info {
		Math::FVector4 Position;
		Math::FVector4 Color;
		float Intensity;
		int32 ShadowIndex;
		float Radius;
		float Padding;
	} Lights[REV_MAX_POINT_LIGHTS];
};

}
