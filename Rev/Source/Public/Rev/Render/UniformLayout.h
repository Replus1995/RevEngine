#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderCore.h"

namespace Rev
{
namespace UL
{

using IndexType = uint16;

//Texture Binding
constexpr IndexType SLinearScreenTex = 1;

constexpr IndexType SPbrStart = 0;
constexpr IndexType SBaseColorTex			= SPbrStart + 0;
constexpr IndexType SMetallicRoughnessTex	= SPbrStart + 1;
constexpr IndexType SNormalTex				= SPbrStart + 2;
constexpr IndexType SOcclusionTex			= SPbrStart + 3;
constexpr IndexType SEmissiveTex			= SPbrStart + 4;

constexpr IndexType SEnviornmentTex = 11;

constexpr IndexType SShadowMap = 5;

//Uniform Buffer Binding
constexpr IndexType BScene = 0;
constexpr IndexType BStaticMesh = 1;
constexpr IndexType BMaterial = 2;
constexpr IndexType BLight = 3;
constexpr IndexType BShadow = 4;

//For Shadow Pass
constexpr IndexType BCascadeShadow = 3;

}
}



