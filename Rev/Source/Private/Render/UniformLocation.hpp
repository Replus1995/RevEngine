#pragma once

namespace Rev
{
//Uniform Location
using ULType = uint16;
constexpr ULType UL_ModelMat = 0;

//Shadow
constexpr ULType UL_ShadowProjMat = 1;
constexpr ULType UL_ShadowViewMat = 2;

//PBR
constexpr ULType UL_PBR_START					= 5;
constexpr ULType UL_BaseColorFactor				= UL_PBR_START;
constexpr ULType UL_Metallic					= UL_PBR_START + 1;
constexpr ULType UL_Roughness					= UL_PBR_START + 2;
constexpr ULType UL_NormalScale					= UL_PBR_START + 3;
constexpr ULType UL_OcclusionStrength			= UL_PBR_START + 4;
constexpr ULType UL_EmissiveFactor				= UL_PBR_START + 5;

constexpr ULType UL_BaseColorTexture			= UL_PBR_START + 6;
constexpr ULType UL_MetallicRoughnessTexture	= UL_PBR_START + 7;
constexpr ULType UL_NormalTexture				= UL_PBR_START + 8;
constexpr ULType UL_OcclusionTexture			= UL_PBR_START + 9;
constexpr ULType UL_EmissiveTexture				= UL_PBR_START + 10;

//

//Uniform Buffer Binding
using UBBType = uint16;
constexpr UBBType UBB_Scene = 0;
constexpr UBBType UBB_Shadow = 1;

}



