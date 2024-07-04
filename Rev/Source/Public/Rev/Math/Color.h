#pragma once
#include "Vector4.h"
#include <memory>

namespace Rev
{
namespace Math
{

struct FColor
{
public:
	uint8_t R, G, B, A;

	FColor();
	FColor(uint8_t InR, uint8_t InG, uint8_t InB, uint8_t InA = 255);

	uint8_t& operator[](int Index);
	uint8_t const& operator[](int Index) const;

	bool operator==(const FColor& InColor) const;
	bool operator!=(const FColor& InColor) const;

	FColor& operator=(const FColor& InColor);
	FColor operator+(const FColor& InColor) const;
	FColor& operator+=(const FColor& InColor);
	FColor operator-(const FColor& InColor) const;
	FColor& operator-=(const FColor& InColor);
};

struct FLinearColor
{
public:
	float R, G, B, A;

	FLinearColor();
	FLinearColor(float InR, float InG, float InB, float InA = 1.0F);
	FLinearColor(const FColor& InColor);
	FLinearColor(const FVector4& InVec);

	operator FVector4() { return FVector4(R, G, B, A); }
	float& operator[](int Index);
	float const& operator[](int Index) const;
	float const* Data() const;

	bool operator==(const FLinearColor& InColor) const;
	bool operator!=(const FLinearColor& InColor) const;

	FLinearColor& operator=(const FLinearColor& InColor);
	FLinearColor operator+(const FLinearColor& InColor) const;
	FLinearColor& operator+=(const FLinearColor& InColor);
	FLinearColor operator-(const FLinearColor& InColor) const;
	FLinearColor& operator-=(const FLinearColor& InColor);
	FLinearColor operator*(const FLinearColor& InColor) const;
	FLinearColor& operator*=(const FLinearColor& InColor);
	FLinearColor operator/(const FLinearColor& InColor) const;
	FLinearColor& operator/=(const FLinearColor& InColor);

	FLinearColor operator*(float InScalar);
	FLinearColor operator*=(float InScalar);
	FLinearColor operator/(float InScalar);
	FLinearColor operator/=(float InScalar);

	FLinearColor Clamped(float InMin = 0.0f, float InMax = 1.0f);

	static FLinearColor RGBA(float InR, float InG, float InB, float InA);
	static FLinearColor BGRA(float InB, float InG, float InR, float InA);

	static FLinearColor FromSRGB(const FColor& InColor);
	static FColor ToSRGB(const FLinearColor& InColor);
};
}
}

#include "Color.inl"