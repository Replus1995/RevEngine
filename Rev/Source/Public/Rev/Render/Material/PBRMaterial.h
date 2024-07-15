#pragma once
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/Texture/Texture.h"

namespace Rev
{

class PBRMaterial : public Material
{
public:
	PBRMaterial();
	virtual ~PBRMaterial();

	Math::FLinearColor BaseColor = Math::FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	Ref<Texture> BaseColorTexture = nullptr;

	float Metallic = 1.0f;
	float Roughness = 1.0f;
	Ref<Texture> MetallicRoughnessTexture = nullptr;

	float NormalScale = 1.0f;
	Ref<Texture> NormalTexture = nullptr;

	float OcclusionStrength = 1.0f;
	Ref<Texture> OcclusionTexture = nullptr;

	Math::FLinearColor EmissiveColor = Math::FLinearColor(0.0f, 0.0f, 0.0f);
	Ref<Texture> EmissiveTexture = nullptr;

protected:
	virtual void UploadUniform() override;
	void UploadTextureUniform(const Ref<class Texture>& InTexture, uint16 InLocation, int TexUnit, int TexUnitFallback = 0);

};

}