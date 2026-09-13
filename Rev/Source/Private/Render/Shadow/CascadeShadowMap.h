#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/UniformDefine.h"
#include "Rev/Render/RenderGraph/RenderGraphDefinitions.h"
#include <array>

namespace Rev
{
class FRGBuilder;
class FSceneProxy;
class FCameraProxy;
class FLightProxy;
class FRHICommandList;
class FRHIShaderProgram;
class FRHIUniformBuffer;
class FRHISamplerState;
class FRHITexture;

class FCascadeShadowMap
{
public:
	void Prepare(const FCameraProxy&, FLightProxy&);
	FRGTextureHandle AddPasses(FRGBuilder&, FSceneProxy*);
	void BindLighting(FRHICommandList&, FRHITexture*);

private:
	void InitializeResources();
	FDirectionalShadowUniform ShadowUniform;
	std::array<Ref<FRHIUniformBuffer>, REV_MAX_CSM_CASCADES> PassUniformBuffers;
	Ref<FRHIUniformBuffer> LightingUniformBuffer;
	Ref<FRHISamplerState> ComparisonSampler;
	Ref<FRHIShaderProgram> ShadowProgram;
	uint8 CascadeCount = 1;
	uint16 Resolution = 2048;
	bool bActive = false;
};
}
