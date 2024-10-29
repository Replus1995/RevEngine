#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderCore.h"


#define REV_BUFFER_BINDING(Index) REV_BINDING_OFFSET_BUFFER + Index
#define REV_TEXTURE_BINDING(Index) REV_BINDING_OFFSET_TEXTURE + Index

namespace Rev
{
namespace UL
{

using IndexType = uint16;

//Texture Binding
constexpr IndexType SLinearScreenTex = 1;

constexpr IndexType SPbrStart = REV_TEXTURE_BINDING(0);
constexpr IndexType SBaseColorTex			= SPbrStart + 0;
constexpr IndexType SMetallicRoughnessTex	= SPbrStart + 1;
constexpr IndexType SNormalTex				= SPbrStart + 2;
constexpr IndexType SOcclusionTex			= SPbrStart + 3;
constexpr IndexType SEmissiveTex			= SPbrStart + 4;

constexpr IndexType SEnviornmentTex = 11;

//Uniform Buffer Binding
constexpr IndexType BScene = REV_BUFFER_BINDING(0);
constexpr IndexType BStaticMesh = REV_BUFFER_BINDING(1);
constexpr IndexType BLight = REV_BUFFER_BINDING(2);
constexpr IndexType BMaterial = REV_BUFFER_BINDING(3);

constexpr IndexType BShadow = REV_BUFFER_BINDING(4);

}
}



