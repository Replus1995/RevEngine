#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{
namespace UL
{

using IndexType = uint16;

//Uniform Location
//Shadow
constexpr IndexType LShadowProjMat = 1;
constexpr IndexType LShadowViewMat = 2;

//PBR
constexpr IndexType LPbrStart			= 5;
constexpr IndexType LBaseColorFactor	= LPbrStart + 0;
constexpr IndexType LMetallic			= LPbrStart + 1;
constexpr IndexType LRoughness			= LPbrStart + 2;
constexpr IndexType LNormalScale		= LPbrStart + 3;
constexpr IndexType LOcclusionStrength	= LPbrStart + 4;
constexpr IndexType LEmissiveFactor		= LPbrStart + 5;

constexpr IndexType LBaseColorTexture	= LPbrStart + 6;


//Sampler Binding
constexpr IndexType SLinearScreenTex = 1;

constexpr IndexType SPbrStart = 5;
constexpr IndexType SBaseColorTex			= SPbrStart + 0;
constexpr IndexType SMetallicRoughnessTex	= LPbrStart + 1;
constexpr IndexType SNormalTex				= LPbrStart + 2;
constexpr IndexType SOcclusionTex			= LPbrStart + 3;
constexpr IndexType SEmissiveTex			= LPbrStart + 4;

constexpr IndexType SEnviornmentTex = 11;

//Uniform Buffer Binding
constexpr IndexType BApp = 0;
constexpr IndexType BScene = 1;
constexpr IndexType BModel = 2;


constexpr IndexType BShadow = 4;
constexpr IndexType BForwardLight = 5;

}
}



