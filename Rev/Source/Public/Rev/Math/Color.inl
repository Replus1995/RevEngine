#pragma once
#include "MathUtils.h"
#include <cassert>
#include <cmath>

namespace Rev
{
namespace Math
{

namespace
{
static constexpr float sGamma = 2.2F;
static constexpr float sInvGamma = 1.0F / 2.2F;
}

inline FColor::FColor()
	: R(0)
	, G(0)
	, B(0)
	, A(255)
{
}

inline FColor::FColor(uint8_t InR, uint8_t InG, uint8_t InB, uint8_t InA)
	: R(InR)
	, G(InG)
	, B(InB)
	, A(InA)
{
}

inline uint8_t& FColor::operator[](int Index)
{
	assert((Index >= 0 && Index < 4) && "Color index out of range");
	switch (Index)
	{
	default:
	case 0:
		return R;
	case 1:
		return G;
	case 2:
		return B;
	case 3:
		return A;
	}
}

inline uint8_t const& FColor::operator[](int Index) const
{
	assert((Index >= 0 && Index < 4) && "Color index out of range");
	switch (Index)
	{
	default:
	case 0:
		return R;
	case 1:
		return G;
	case 2:
		return B;
	case 3:
		return A;
	}
}

inline uint8_t const* FColor::Data() const
{
	return &R;
}

inline bool FColor::operator==(const FColor& InColor) const
{
	return R == InColor.R && G == InColor.G && B == InColor.B && A == InColor.A;
}

inline bool FColor::operator!=(const FColor& InColor) const
{
	return R != InColor.R || G != InColor.G || B != InColor.B || A != InColor.A;
}

inline FColor& FColor::operator=(const FColor& InColor)
{
	R = InColor.R;
	G = InColor.G;
	B = InColor.B;
	A = InColor.A;
	return *this;
}

inline FColor FColor::operator+(const FColor& InColor) const
{
	FColor Result = *this;
	Result += InColor;
	return Result;
}

inline FColor& FColor::operator+=(const FColor& InColor)
{
	R = (uint8_t)Clamp<uint32_t>((uint32_t)R + (uint32_t)InColor.R, 0, 255);
	G = (uint8_t)Clamp<uint32_t>((uint32_t)G + (uint32_t)InColor.G, 0, 255);
	B = (uint8_t)Clamp<uint32_t>((uint32_t)B + (uint32_t)InColor.B, 0, 255);
	A = (uint8_t)Clamp<uint32_t>((uint32_t)A + (uint32_t)InColor.A, 0, 255);
	return *this;
}

inline FColor FColor::operator-(const FColor& InColor) const
{
	FColor Result = *this;
	Result -= InColor;
	return Result;
}

inline FColor& FColor::operator-=(const FColor& InColor)
{
	R = (uint8_t)Clamp<uint32_t>((uint32_t)R - (uint32_t)InColor.R, 0, 255);
	G = (uint8_t)Clamp<uint32_t>((uint32_t)G - (uint32_t)InColor.G, 0, 255);
	B = (uint8_t)Clamp<uint32_t>((uint32_t)B - (uint32_t)InColor.B, 0, 255);
	A = (uint8_t)Clamp<uint32_t>((uint32_t)A - (uint32_t)InColor.A, 0, 255);
	return *this;
}

inline FLinearColor::FLinearColor()
	: R(0.0F)
	, G(0.0F)
	, B(0.0F)
	, A(1.0F)
{
}

inline FLinearColor::FLinearColor(float InR, float InG, float InB, float InA)
	: R(InR)
	, G(InG)
	, B(InB)
	, A(InA)
{
}

//inline FLinearColor::FLinearColor(const FColor& InColor)
//	: R(std::powf((float)InColor.R / 255.0f, sGamma))
//	, G(std::powf((float)InColor.G / 255.0f, sGamma))
//	, B(std::powf((float)InColor.B / 255.0f, sGamma))
//	, A((float)InColor.A / 255.0f)
//{
//}

inline FLinearColor::FLinearColor(const FVector4& InVec)
	: R(InVec.X)
	, G(InVec.Y)
	, B(InVec.Z)
	, A(InVec.W)
{
}

inline float& FLinearColor::operator[](int Index)
{
	assert((Index >= 0 && Index < 4) && "LinearColor index out of range");
	switch (Index)
	{
	default:
	case 0:
		return R;
	case 1:
		return G;
	case 2:
		return B;
	case 3:
		return A;
	}
}

inline float const& FLinearColor::operator[](int Index) const
{
	assert((Index >= 0 && Index < 4) && "LinearColor index out of range");
	switch (Index)
	{
	default:
	case 0:
		return R;
	case 1:
		return G;
	case 2:
		return B;
	case 3:
		return A;
	}
}

inline float const* FLinearColor::Data() const
{
	return &R;
}

inline bool FLinearColor::operator==(const FLinearColor& InColor) const
{
	return R == InColor.R && G == InColor.G && B == InColor.B && A == InColor.A;
}

inline bool FLinearColor::operator!=(const FLinearColor& InColor) const
{
	return R != InColor.R || G != InColor.G || B != InColor.B || A != InColor.A;
}

inline FLinearColor& FLinearColor::operator=(const FLinearColor& InColor)
{
	R = InColor.R;
	G = InColor.G;
	B = InColor.B;
	A = InColor.A;
	return *this;
}

inline FLinearColor FLinearColor::operator+(const FLinearColor& InColor) const
{
	return FLinearColor(R + InColor.R, G + InColor.G, B + InColor.B, A + InColor.A);
}

inline FLinearColor& FLinearColor::operator+=(const FLinearColor& InColor)
{
	R += InColor.R;
	G += InColor.G;
	B += InColor.B;
	A += InColor.A;
	return *this;
}

inline FLinearColor FLinearColor::operator-(const FLinearColor& InColor) const
{
	return FLinearColor(R - InColor.R, G - InColor.G, B - InColor.B, A - InColor.A);
}

inline FLinearColor& FLinearColor::operator-=(const FLinearColor& InColor)
{
	R -= InColor.R;
	G -= InColor.G;
	B -= InColor.B;
	A -= InColor.A;
	return *this;
}

inline FLinearColor FLinearColor::operator*(const FLinearColor& InColor) const
{
	return FLinearColor(R * InColor.R, G * InColor.G, B * InColor.B, A * InColor.A);
}

inline FLinearColor& FLinearColor::operator*=(const FLinearColor& InColor)
{
	R *= InColor.R;
	G *= InColor.G;
	B *= InColor.B;
	A *= InColor.A;
	return *this;
}

inline FLinearColor FLinearColor::operator/(const FLinearColor& InColor) const
{
	return FLinearColor(R / InColor.R, G / InColor.G, B / InColor.B, A / InColor.A);
}

inline FLinearColor& FLinearColor::operator/=(const FLinearColor& InColor)
{
	R /= InColor.R;
	G /= InColor.G;
	B /= InColor.B;
	A /= InColor.A;
	return *this;
}

inline FLinearColor FLinearColor::operator*(float InScalar)
{
	return FLinearColor(R * InScalar, G * InScalar, B * InScalar, A * InScalar);
}

inline FLinearColor FLinearColor::operator*=(float InScalar)
{
	R *= InScalar;
	G *= InScalar;
	B *= InScalar;
	A *= InScalar;
	return *this;
}

inline FLinearColor FLinearColor::operator/(float InScalar)
{
	const float InvScalar = 1.0F / InScalar;
	return FLinearColor(R * InvScalar, G * InvScalar, B * InvScalar, A * InvScalar);
}

inline FLinearColor FLinearColor::operator/=(float InScalar)
{
	const float InvScalar = 1.0F / InScalar;
	R *= InvScalar;
	G *= InvScalar;
	B *= InvScalar;
	A *= InvScalar;
	return *this;
}

inline FLinearColor FLinearColor::Clamped(float InMin, float InMax)
{
	FLinearColor Result;
	Result.R = (InMin, InMax);
	return Result;
}

inline FLinearColor FLinearColor::RGBA(float InR, float InG, float InB, float InA)
{
	return FLinearColor(InR, InG, InB, InA);
}

inline FLinearColor FLinearColor::BGRA(float InB, float InG, float InR, float InA)
{
	return FLinearColor(InR, InG, InB, InA);
}

inline FLinearColor FLinearColor::FromSRGB(const FColor& InColor)
{
	FLinearColor Result;
	Result.R = std::powf((float)InColor.R / 255.0f, sGamma);
	Result.G = std::powf((float)InColor.G / 255.0f, sGamma);
	Result.B = std::powf((float)InColor.B / 255.0f, sGamma);
	Result.A = (float)InColor.A / 255.0f;
	return Result;
}

inline FColor FLinearColor::ToSRGB(const FLinearColor& InColor)
{
	FColor Result;
	Result.R = (uint8_t)Clamp<float>(std::powf(InColor.R, sInvGamma) * 255.0F, 0, 255);
	Result.G = (uint8_t)Clamp<float>(std::powf(InColor.G, sInvGamma) * 255.0F, 0, 255);
	Result.B = (uint8_t)Clamp<float>(std::powf(InColor.B, sInvGamma) * 255.0F, 0, 255);
	Result.A = (uint8_t)Clamp<float>(InColor.A * 255.0F, 0, 255);
	return Result;
}

inline FLinearColor FLinearColor::FromColor(const FColor& InColor, bool bSRGB)
{
	if (bSRGB)
	{
		return FromSRGB(InColor);
	}
	else
	{
		FLinearColor Result;
		Result.R = (float)InColor.R / 255.0f;
		Result.G = (float)InColor.G / 255.0f;
		Result.B = (float)InColor.B / 255.0f;
		Result.A = (float)InColor.A / 255.0f;
		return Result;
	}
}

inline FColor FLinearColor::ToColor(const FLinearColor& InColor, bool bSRGB)
{
	if (bSRGB)
	{
		return ToSRGB(InColor);
	}
	else
	{
		FColor Result;
		Result.R = (uint8_t)Clamp<float>(InColor.R * 255.0F, 0, 255);
		Result.G = (uint8_t)Clamp<float>(InColor.G * 255.0F, 0, 255);
		Result.B = (uint8_t)Clamp<float>(InColor.B * 255.0F, 0, 255);
		Result.A = (uint8_t)Clamp<float>(InColor.A * 255.0F, 0, 255);
		return Result;
	}
}

inline FLinearColor FLinearColor::FromTemperature(float InTemperature)
{
	float Temp = Clamp(InTemperature, 1000.0f, 15000.0f);

	// Approximate Planckian locus in CIE 1960 UCS
	float u = (0.860117757f + 1.54118254e-4f * Temp + 1.28641212e-7f * Temp * Temp) / (1.0f + 8.42420235e-4f * Temp + 7.08145163e-7f * Temp * Temp);
	float v = (0.317398726f + 4.22806245e-5f * Temp + 4.20481691e-8f * Temp * Temp) / (1.0f - 2.89741816e-5f * Temp + 1.61456053e-7f * Temp * Temp);

	float x = 3.0f * u / (2.0f * u - 8.0f * v + 4.0f);
	float y = 2.0f * v / (2.0f * u - 8.0f * v + 4.0f);
	float z = 1.0f - x - y;

	float Y = 1.0f;
	float X = Y / y * x;
	float Z = Y / y * z;

	// XYZ to RGB with BT.709 primaries
	float R = 3.2404542f * X + -1.5371385f * Y + -0.4985314f * Z;
	float G = -0.9692660f * X + 1.8760108f * Y + 0.0415560f * Z;
	float B = 0.0556434f * X + -0.2040259f * Y + 1.0572252f * Z;

	return FLinearColor(R, G, B);
}

}
}