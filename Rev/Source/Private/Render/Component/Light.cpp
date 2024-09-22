#include "Rev/Render/Component/Light.h"

namespace Rev
{

Light::Light()
	: mIntensity(1.0F)
	, mColor(0.8F, 0.8F, 0.6F, 1.0F)
{
}

Light::Light(float InIntensity, const Math::FLinearColor& InColor)
	: mIntensity(InIntensity)
	, mColor(InColor)
{
}
Light::~Light()
{
}

void Light::SetColorByTemperature(float InTemperature)
{
	mColor = Math::FLinearColor::FromTemperature(InTemperature);
}

DirectionalLight::DirectionalLight()
	: Light()
{
}

DirectionalLight::DirectionalLight(float InDistance, float InIntensity, const Math::FLinearColor& InColor)
	: Light(InIntensity, InColor)
{
}

DirectionalLight::~DirectionalLight()
{
}


SpotLight::SpotLight()
	: Light()
	, mRadius(10.0F)
	, mAngle(30.0F)
{
}

SpotLight::SpotLight(float InDistance, float InAngle, float InIntensity, const Math::FLinearColor& InColor)
	: Light(InIntensity, InColor)
	, mRadius(InDistance)
	, mAngle(InAngle)
{
}

SpotLight::~SpotLight()
{
}

PointLight::PointLight()
	: Light()
	, mRadius(10.0F)
{
}

PointLight::PointLight(float InRadius, float InIntensity, const Math::FLinearColor& InColor)
	: Light(InIntensity, InColor)
	, mRadius(InRadius)
{
}

PointLight::~PointLight()
{
}

}


