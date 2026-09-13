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
constexpr IndexType SGBufferA = 5;
constexpr IndexType SGBufferB = 6;
constexpr IndexType SGBufferC = 7;
constexpr IndexType SSceneDepth = 8;
constexpr IndexType SSceneColor = 9;
constexpr IndexType SDirectionalShadow = 10;

//Uniform Buffer Binding
constexpr IndexType BScene = 0;
constexpr IndexType BStaticMesh = 1;
constexpr IndexType BLight = 2;
constexpr IndexType BMaterial = 3;

constexpr IndexType BShadow = 4;
constexpr IndexType BDeferredDebug = 5;

}
}



