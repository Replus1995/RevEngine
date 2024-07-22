#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/RenderCore.h"

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

struct FForwardLightUniform
{

};

struct FDeferredLightUniform
{

};

}