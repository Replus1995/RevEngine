#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/Component/Light.h"

namespace Rev
{

struct alignas(16) FShadowViewUniform
{
	Math::FMatrix4 LightProjMats[REV_MAX_SHADOW_VIEWS];
	Math::FMatrix4 LightViewMats[REV_MAX_SHADOW_VIEWS];
};

struct FDirectionalLightUniform
{
	struct alignas(16) Info {
		Math::FMatrix4 ProjMats[REV_MAX_SHADOW_VIEWS];
		Math::FMatrix4 ViewMats[REV_MAX_SHADOW_VIEWS];
		Math::FVector4 Direction;
		Math::FVector4 Color;
		float Intensity = 0.0f;
		uint32 ShadowMapCount = 0;
	}  Lights[REV_MAX_DIRECTIONAL_LIGHTS];
	uint32 Count = 0;
};

struct FSpotLightUniform
{
	uint32 Count = 0;
	struct alignas(16) Info {
		Math::FVector4 Position;
		Math::FVector4 Direction;
		Math::FVector4 Color;
		float Intensity;
		int32 ShadowIndex;
		float Radius;
		float Angle;
	} Lights[REV_MAX_SPOT_LIGHTS];
};

struct FPointLightUniform
{
	uint32 Count = 0;
	struct alignas(16) Info {
		Math::FVector4 Position;
		Math::FVector4 Color;
		float Intensity;
		int32 ShadowIndex;
		float Radius;
		float Padding;
	} Lights[REV_MAX_POINT_LIGHTS];
};

struct FShadowRenderData
{
	FRHITextureRef Texture = nullptr;
	FRHIUniformBufferRef ViewUB = nullptr;
};

class FScene;
class FRHICommandList;
class FRHIUniformBuffer;
class FLightProxy
{
public:
	FLightProxy();
	~FLightProxy();

	void Prepare(const Ref<FScene>& Scene);
	void SyncResource(FRHICommandList& RHICmdList);

	uint32 GetDirectionalLightCount() const { return DirectionalLightParams.Count; }
	FRHITexture* GetDirectionalShadowMap(uint32 Index);
	void BeginDrawDirectionalShadowMap(FRHICommandList& RHICmdList, uint32 Index);
	void EndDrawDirectionalShadowMap();


private:
	Ref<FRHIUniformBuffer> mLightUB;
	FDirectionalLightUniform DirectionalLightParams;
	FShadowRenderData DirectionalShadowData[REV_MAX_DIRECTIONAL_LIGHTS];

	Ref<class FShadowMapMaterial> ShadowMapMat = nullptr;
};

}