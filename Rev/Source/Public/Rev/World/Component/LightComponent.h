#pragma once
#include "Rev/Render/Component/Light.h"


namespace Rev
{

class FRHIRenderTarget;
struct DirectionalLightComponent
{
	DirectionalLight Light;
	Ref<FRHIRenderTarget> mShadowDepth = nullptr;

	DirectionalLightComponent() = default;
	DirectionalLightComponent(const DirectionalLightComponent& Other) { this->Light = Other.Light; }
};

struct SpotLightComponent
{
	SpotLight Light;
	Ref<FRHIRenderTarget> mShadowDepth = nullptr;

	SpotLightComponent() = default;
	SpotLightComponent(const SpotLightComponent& Other) { this->Light = Other.Light; }
};

struct PointLightComponent
{
	PointLight Light;
	Ref<FRHIRenderTarget> mShadowDepth = nullptr;

	PointLightComponent() = default;
	PointLightComponent(const PointLightComponent& Other) { this->Light = Other.Light; }
};


}