#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{
namespace UL
{

using IndexType = uint16;

//Sampler Binding
constexpr IndexType SLinearScreenTex = 1;

constexpr IndexType SPbrStart = 0;
constexpr IndexType SBaseColorTex			= SPbrStart + 0;
constexpr IndexType SMetallicRoughnessTex	= SPbrStart + 1;
constexpr IndexType SNormalTex				= SPbrStart + 2;
constexpr IndexType SOcclusionTex			= SPbrStart + 3;
constexpr IndexType SEmissiveTex			= SPbrStart + 4;

constexpr IndexType SEnviornmentTex = 11;

//Uniform Buffer Binding
constexpr IndexType BScene = 0;
constexpr IndexType BStaticMesh = 1;
constexpr IndexType BLight = 2;
constexpr IndexType BMaterial = 3;

constexpr IndexType BShadow = 4;

}
}



