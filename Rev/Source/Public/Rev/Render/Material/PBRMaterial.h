#pragma once
#include "Rev/Render/Material/Material.h"
#include "Rev/Math/Maths.h"

namespace Rev
{

class FTexture;
class FRHIUniformBuffer;
struct alignas(16) FPBRMaterialUniform
{
	Math::FLinearColor BaseColorFactor = Math::FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	float Metallic = 0.0f;
	float Roughness = 1.0f;
	float NormalScale = 1.0f;
	float OcclusionStrength = 1.0f;
	Math::FVector3 EmissiveFactor = Math::FVector3(0.0f, 0.0f, 0.0f);
};


class REV_API FPBRMaterial : public FMaterial
{
public:
	FPBRMaterial();
	virtual ~FPBRMaterial();

	virtual void Compile() override;
	virtual void PreDraw(FRHICommandList& RHICmdList) override;

	FPBRMaterialUniform PBRMaterialParams;
	Ref<FTexture> BaseColorTexture = nullptr;
	Ref<FTexture> MetallicRoughnessTexture = nullptr;
	Ref<FTexture> NormalTexture = nullptr;
	Ref<FTexture> OcclusionTexture = nullptr;
	Ref<FTexture> EmissiveTexture = nullptr;

protected:
	Ref<FRHIUniformBuffer> MaterialUB;
};

}