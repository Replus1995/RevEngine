#pragma once
#include "Rev/Render/Light.h"


namespace Rev
{

class FRHIRenderTarget;
struct DirectionalLightComponent
{
	DirectionalLight Light;
	Ref<FRHIRenderTarget> mShadowDepth = nullptr;

	DirectionalLightComponent() = default;
	DirectionalLightComponent(const DirectionalLightComponent& other) { this->Light = other.Light; }
};

struct SpotLightComponent
{
	SpotLight Light;
	Ref<FRHIRenderTarget> mShadowDepth = nullptr;

	SpotLightComponent() = default;
	SpotLightComponent(const SpotLightComponent& other) { this->Light = other.Light; }
};

struct PointLightComponent
{
	PointLight Light;
	Ref<FRHIRenderTarget> mShadowDepth = nullptr;

	PointLightComponent() = default;
	PointLightComponent(const PointLightComponent& other) { this->Light = other.Light; }
};


}