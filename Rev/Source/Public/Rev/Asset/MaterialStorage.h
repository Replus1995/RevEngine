#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Math/Maths.h"

namespace Rev
{
struct FTexture;
struct FMaterialStorage
{
public:
	//std::string Name;
	EMaterialBlendMode BlendMode = MBM_Opaque;
	EMaterialShadingModel ShadingModel = MSM_Default;
	float MaskClip = 0.5f;
	bool TwoSided = false;

	virtual Ref<FMaterial> CreateMaterial() { return mCache; }
protected:
	Ref<FMaterial> mCache = nullptr;
};

struct FPBRMaterialStorage : public FMaterialStorage
{
public:
	Math::FLinearColor BaseColorFactor = Math::FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	Ref<FTexture> BaseColorTexture = nullptr;

	float Metallic = 1.0f;
	float Roughness = 1.0f;
	Ref<FTexture> MetallicRoughnessTexture = nullptr;

	float NormalScale = 1.0f;
	Ref<FTexture> NormalTexture = nullptr;

	float OcclusionStrength = 1.0f;
	Ref<FTexture> OcclusionTexture = nullptr;

	Math::FVector3 EmissiveFactor = Math::FVector3(0.0f, 0.0f, 0.0f);
	Ref<FTexture> EmissiveTexture = nullptr;

	virtual Ref<FMaterial> CreateMaterial() override;
};


}