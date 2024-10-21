#pragma once
#include "Rev/Render/Material/SurfaceMaterial.h"
#include "Rev/Math/Maths.h"

namespace Rev
{

class FTexture;
class REV_API PBRMaterial : public SurfaceMaterial
{
public:
	PBRMaterial();
	virtual ~PBRMaterial();

	virtual void Compile() override;
	virtual void PreDraw(FRHICommandList& RHICmdList) override;

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

};

}