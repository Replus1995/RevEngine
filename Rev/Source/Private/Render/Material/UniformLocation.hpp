#pragma once

namespace Rev
{
using ULType = uint16;
constexpr ULType UL_Transform = 0;

//PBR
constexpr ULType UL_PBR_START					= 1;
constexpr ULType UL_BaseColor					= UL_PBR_START;
constexpr ULType UL_BaseColorTexture			= UL_PBR_START + 1;
constexpr ULType UL_Metallic					= UL_PBR_START + 2;
constexpr ULType UL_Roughness					= UL_PBR_START + 3;
constexpr ULType UL_MetallicRoughnessTexture	= UL_PBR_START + 4;
constexpr ULType UL_NormalScale					= UL_PBR_START + 5;
constexpr ULType UL_NormalTexture				= UL_PBR_START + 6;
constexpr ULType UL_OcclusionStrength			= UL_PBR_START + 7;
constexpr ULType UL_OcclusionTexture			= UL_PBR_START + 8;
constexpr ULType UL_EmissiveColor				= UL_PBR_START + 9;
constexpr ULType UL_EmissiveTexture				= UL_PBR_START + 10;



}



