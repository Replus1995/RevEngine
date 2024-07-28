#pragma once
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/Texture/Texture.h"

namespace Rev
{

class REV_API PBRMaterial : public Material
{
public:
	PBRMaterial();
	virtual ~PBRMaterial();

	virtual void Compile() override;
	virtual void PreDraw() override;

	Math::FLinearColor BaseColorFactor = Math::FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	Ref<Texture> BaseColorTexture = nullptr;

	float Metallic = 1.0f;
	float Roughness = 1.0f;
	Ref<Texture> MetallicRoughnessTexture = nullptr;

	float NormalScale = 1.0f;
	Ref<Texture> NormalTexture = nullptr;

	float OcclusionStrength = 1.0f;
	Ref<Texture> OcclusionTexture = nullptr;

	Math::FVector3 EmissiveFactor = Math::FVector3(0.0f, 0.0f, 0.0f);
	Ref<Texture> EmissiveTexture = nullptr;

};

}