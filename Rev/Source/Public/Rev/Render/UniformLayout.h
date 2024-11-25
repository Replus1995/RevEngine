#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderCore.h"

namespace Rev
{
using IndexType = uint16;

namespace TB
{
//Texture Binding

constexpr IndexType PbrTexBegin = 0;
constexpr IndexType BaseColorTex			= PbrTexBegin + 0;
constexpr IndexType MetallicRoughnessTex	= PbrTexBegin + 1;
constexpr IndexType NormalTex				= PbrTexBegin + 2;
constexpr IndexType OcclusionTex			= PbrTexBegin + 3;
constexpr IndexType EmissiveTex				= PbrTexBegin + 4;

constexpr IndexType EnviornmentTex = 11;

constexpr IndexType DirectionalShadowMapBegin = 20;

}

namespace UB
{
//Uniform Buffer Binding
constexpr IndexType Scene = 0;
constexpr IndexType StaticMesh = 1;
constexpr IndexType Light = 2;
constexpr IndexType Material = 3;

constexpr IndexType ShadowView = 0;

}

}



