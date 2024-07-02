#pragma once

namespace Rev
{
namespace Math
{

template<typename T>
inline T PI() noexcept;

template<typename T>
inline T Radians(T InDegree);

template<typename T>
inline T Degrees(T InRadian);

template<typename T>
inline T Clamp(T InVal, T InMin, T InMax);

}
}

#include "MathUtils.inl"