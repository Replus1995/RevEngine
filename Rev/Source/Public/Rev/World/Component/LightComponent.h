#pragma once
#include "Rev/Render/Component/Light.h"


namespace Rev
{

struct DirectionalLightComponent
{
	DirectionalLight Light;

	DirectionalLightComponent() = default;
	DirectionalLightComponent(const DirectionalLightComponent& Other) { this->Light = Other.Light; }
};

struct SpotLightComponent
{
	SpotLight Light;

	SpotLightComponent() = default;
	SpotLightComponent(const SpotLightComponent& Other) { this->Light = Other.Light; }
};

struct PointLightComponent
{
	PointLight Light;

	PointLightComponent() = default;
	PointLightComponent(const PointLightComponent& Other) { this->Light = Other.Light; }
};


}
