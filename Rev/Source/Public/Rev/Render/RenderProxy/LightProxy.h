#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/Component/Light.h"
#include "Rev/Render/RHI/RHIDefinitions.h"

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
		int32 ShadowMapIndex = -1;
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
	FRHIRenderPassRef RenderPass = nullptr;
	FRHITextureRef Texture = nullptr;
	FRHIUniformBufferRef ViewUniform = nullptr;

	void InitRHI(uint32 ShadowMapLayers);
	void Reset();
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

	void BeginDirectionalShadowPass(FRHICommandList& RHICmdList, uint32 Index);


	void EndShadowPass(FRHICommandList& RHICmdList);


private:
	Ref<FRHIUniformBuffer> mLightUB;
	FDirectionalLightUniform DirectionalLightParams;
	FShadowRenderData DirectionalShadowData[REV_MAX_DIRECTIONAL_LIGHTS];

	FShadowRenderData* LastShadowRenderData = nullptr;
	Ref<class FShadowMapMaterial> ShadowMapMat = nullptr;
};

}