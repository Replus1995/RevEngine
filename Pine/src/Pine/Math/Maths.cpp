#include "pinepch.h"
#include "Maths.h"
#include <cmath>

namespace Pine
{

static constexpr double sDegreeToRadianFactor = 0.01745329251994329576923690768489;
static constexpr double sRadianToDegreeFactor = 57.295779513082320876798154814105;

template<typename T>
T FMaths::Radians(T InDegree) noexcept
{
	return InDegree * (T)sDegreeToRadianFactor;
}

template<typename T>
T FMaths::Degrees(T InRadian) noexcept
{
	return InRadian * (T)sRadianToDegreeFactor;
}

float FMaths::PI() noexcept
{
	return 3.14159F;
}

}