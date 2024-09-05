#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/Material/SurfaceMaterial.h"
#include "Rev/Math/Maths.h"

namespace Rev
{
struct FTextureStorage;
struct FSurfaceMaterialStorage
{
public:
	std::string Name;
	EMaterialBlendMode BlendMode = MBM_Opaque;
	EMaterialShadingModel ShadingModel = MSM_Default;
	float MaskClip = 0.5f;
	bool TwoSided = false;

	virtual Ref<SurfaceMaterial> CreateMaterial() { return mCache; }
protected:
	Ref<SurfaceMaterial> mCache = nullptr;
};

struct FPBRMaterialStorage : public FSurfaceMaterialStorage
{
public:
	Math::FLinearColor BaseColorFactor = Math::FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	Ref<FTextureStorage> BaseColorTexture = nullptr;

	float Metallic = 1.0f;
	float Roughness = 1.0f;
	Ref<FTextureStorage> MetallicRoughnessTexture = nullptr;

	float NormalScale = 1.0f;
	Ref<FTextureStorage> NormalTexture = nullptr;

	float OcclusionStrength = 1.0f;
	Ref<FTextureStorage> OcclusionTexture = nullptr;

	Math::FVector3 EmissiveFactor = Math::FVector3(0.0f, 0.0f, 0.0f);
	Ref<FTextureStorage> EmissiveTexture = nullptr;

	virtual Ref<SurfaceMaterial> CreateMaterial() override;
};


}