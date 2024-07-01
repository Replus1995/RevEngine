#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "Rotator.h"
#include "Quaternion.h"
#include "Matrix3.h"
#include "Matrix4.h"

namespace Pine
{

class FMaths
{
public:
	static PINE_API float PI() noexcept;
	template<typename T>
	static T Radians(T InDegree) noexcept;
	template<typename T>
	static T Degrees(T InRadian) noexcept;
	template<typename T>
	static T Clamp(T InVal, T InMin, T InMax) noexcept
	{
		return InVal < InMin ? InMin : InVal > InMax ? InMax : InVal;
	}

};

template PINE_API float FMaths::Radians(float InDegree) noexcept;
template PINE_API double FMaths::Radians(double InDegree) noexcept;
template PINE_API float FMaths::Degrees(float InRadian) noexcept;
template PINE_API double FMaths::Degrees(double InRadian) noexcept;
}