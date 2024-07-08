#pragma once

namespace Rev
{
namespace Math
{

namespace
{
	static constexpr double sDegreeToRadianFactor = 0.01745329251994329576923690768489;
	static constexpr double sRadianToDegreeFactor = 57.295779513082320876798154814105;
	static constexpr double sPI = 3.141592653589793;
}

template<typename T>
T PI() noexcept
{
	return T(sPI);
}

template<typename T>
T Radians(T InDegree)
{
	return InDegree * (T)sDegreeToRadianFactor;
}

template<typename T>
T Degrees(T InRadian)
{
	return InRadian * (T)sRadianToDegreeFactor;
}

template<typename T>
T Min(T InValA, T InValB)
{
	return InValA < InValB ? InValA : InValB;
}

template<typename T>
T Max(T InValA, T InValB)
{
	return InValA > InValB ? InValA : InValB;
}

template<typename T>
T Clamp(T InVal, T InMin, T InMax)
{
	return InVal < InMin ? InMin : InVal > InMax ? InMax : InVal;
}

}
}