#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"

namespace Rev
{
class FRHIRenderTarget;
enum ELightKind : uint8
{
	LK_Directional,
	LK_Spot,
	LK_Point
};

class REV_API Light
{
public:
	Light();
	Light(float InIntensity, const Math::FLinearColor& InColor);
	virtual ~Light();

	virtual ELightKind GetKind() const = 0;
	float GetIntensity() const { return mIntensity; }
	void SetIntensity(float InIntensity) { mIntensity = InIntensity; }
	const Math::FLinearColor& GetColor() const { return mColor; }
	void SetIntensity(const Math::FLinearColor& InColor) { mColor = InColor; }
	 
protected:
	Math::FLinearColor mColor;
	float mIntensity;
};

class REV_API DirectionalLight : public Light
{
public:
	DirectionalLight();
	DirectionalLight(float InDistance, float InIntensity, const Math::FLinearColor& InColor);
	virtual ~DirectionalLight();
	virtual ELightKind GetKind() const override { return LK_Directional; }

	float GetDistance() const { return mDistance; }
	void SetDistance(float InDistance) { mDistance = InDistance; }

private:
	float mDistance;
};

class REV_API SpotLight : public Light
{
public:
	SpotLight();
	SpotLight(float InDistance, float InAngle, float InIntensity, const Math::FLinearColor& InColor);
	virtual ~SpotLight();
	virtual ELightKind GetKind() const override { return LK_Spot; }

	float GetDistance() const { return mDistance; }
	void SetDistance(float InDistance) { mDistance = InDistance; }
	float GetAngle() const { return mAngle; }
	void SetAngle(float InAngle) { mAngle = InAngle; }

private:
	float mDistance;
	float mAngle; //In Degrees
};


class REV_API PointLight : public Light
{
public:
	PointLight();
	PointLight(float InRadius, float InIntensity, const Math::FLinearColor& InColor);
	virtual ~PointLight();
	virtual ELightKind GetKind() const override { return LK_Point; }

	float GetRadius() const { return mRadius; }
	void SetRadius(float InRadius) { mRadius = InRadius; }

private:
	float mRadius;
};




}