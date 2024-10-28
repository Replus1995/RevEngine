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
	Math::FMatrix4 InvViewProjMat;
};

struct alignas(16) FStaticMeshUniform
{
	Math::FMatrix4 ModelMat;
};

struct FShadowUniform
{
	Math::FMatrix4 LightProjMats[6];
	Math::FMatrix4 LightViewMats[6];
	Math::FVector4 LightParams; //X = ViewCount
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