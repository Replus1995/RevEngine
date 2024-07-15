#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Math/Maths.h"

namespace Rev
{
struct FTextureStorage;
struct FMaterialStorage
{
	std::string Name;
	EMaterialDomain Domain = Rev::MD_Surface;
	EBlendMode BlendMode = Rev::BM_Opaque;
	EShadingModel ShadingModel = Rev::SM_Default;
	float MaskClip = 0.5f;
	bool TwoSided = false;
};

struct FPBRMaterialStorage : public FMaterialStorage
{
	Math::FLinearColor BaseColor = Math::FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	Ref<FTextureStorage> BaseColorTexture = nullptr;

	float Metallic = 1.0f;
	float Roughness = 1.0f;
	Ref<FTextureStorage> MetallicRoughnessTexture = nullptr;

	float NormalScale = 1.0f;
	Ref<FTextureStorage> NormalTexture = nullptr;

	float OcclusionStrength = 1.0f;
	Ref<FTextureStorage> OcclusionTexture = nullptr;

	Math::FLinearColor EmissiveColor = Math::FLinearColor(0.0f, 0.0f, 0.0f);
	Ref<FTextureStorage> EmissiveTexture = nullptr;

};


}