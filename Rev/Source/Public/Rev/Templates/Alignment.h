#pragma once
#include "Rev/Core/Base.h"

template <typename T>
FORCEINLINE constexpr T Align(T Val, uint64 Alignment)
{
	static_assert(std::is_integral<T>::value || std::is_pointer<T>::value, "Align expects integer or pointer type");
	return (T)(((uint64)Val + Alignment - 1) & ~(Alignment - 1));
}

template <typename T>
FORCEINLINE constexpr T AlignDown(T Val, uint64 Alignment)
{
	static_assert(std::is_integral<T>::value || std::is_pointer<T>::value, "AlignDown expects integer or pointer type");

	return (T)(((uint64)Val) & ~(Alignment - 1));
}

template <typename T>
FORCEINLINE constexpr bool IsAligned(T Val, uint64 Alignment)
{
	static_assert(std::is_integral<T>::value || std::is_pointer<T>::value, "IsAligned expects integer or pointer type");

	return !((uint64)Val & (Alignment - 1));
}

template <typename T>
FORCEINLINE constexpr T AlignArbitrary(T Val, uint64 Alignment)
{
	static_assert(std::is_integral<T>::value || std::is_pointer<T>::value, "AlignArbitrary expects integer or pointer type");

	return (T)((((uint64)Val + Alignment - 1) / Alignment) * Alignment);
}