#pragma once
#include <cassert>
#include <cmath>

namespace Rev
{
namespace Math
{

template<typename T>
inline TVector2<T>::TVector2()
	: X(0.0F)
	, Y(0.0F)
{
}

template<typename T>
inline TVector2<T>::TVector2(T InScalar)
	: X(InScalar)
	, Y(InScalar)
{
}

template<typename T>
inline TVector2<T>::TVector2(T InX, T InY)
	: X(InX)
	, Y(InY)
{
}

template<typename T>
inline TVector2<T>::TVector2(const TVector2<T>& InVec)
	: X(InVec.X)
	, Y(InVec.Y)
{
}

template<typename T>
inline T& TVector2<T>::operator[](int Index)
{
	assert((Index >= 0 && Index < 2) && "Vector2 index out of range");
	switch (Index)
	{
	default:
	case 0:
		return X;
	case 1:
		return Y;
	}
}

template<typename T>
inline T const& TVector2<T>::operator[](int Index) const
{
	assert((Index >= 0 && Index < 2) && "Vector2 index out of range");
	switch (Index)
	{
	default:
	case 0:
		return X;
	case 1:
		return Y;
	}
}

template<typename T>
inline T const* TVector2<T>::Data() const
{
	return &X;
}

template<typename T>
inline bool TVector2<T>::operator==(const TVector2<T>& InVec) const
{
	return X == InVec.X && Y == InVec.Y;
}

template<typename T>
inline bool TVector2<T>::operator!=(const TVector2<T>& InVec) const
{
	return X != InVec.X || Y != InVec.Y;
}

template<typename T>
inline TVector2<T> TVector2<T>::operator-() const
{
	return TVector2<T>(-X, -Y);
}

template<typename T>
inline TVector2<T>& TVector2<T>::operator=(const TVector2<T>& InVec)
{
	X = InVec.X;
	Y = InVec.Y;
	return *this;
}

template<typename T>
inline TVector2<T> TVector2<T>::operator+(const TVector2<T>& InVec) const
{
	return TVector2<T>(X + InVec.X, Y + InVec.Y);
}

template<typename T>
inline TVector2<T>& TVector2<T>::operator+=(const TVector2<T>& InVec)
{
	X += InVec.X;
	Y += InVec.Y;
	return *this;
}

template<typename T>
inline TVector2<T> TVector2<T>::operator-(const TVector2<T>& InVec) const
{
	return TVector2<T>(X - InVec.X, Y - InVec.Y);
}

template<typename T>
inline TVector2<T>& TVector2<T>::operator-=(const TVector2<T>& InVec)
{
	X -= InVec.X;
	Y -= InVec.Y;
	return *this;
}

template<typename T>
inline TVector2<T> TVector2<T>::operator*(const TVector2<T>& InVec) const
{
	return TVector2<T>(X * InVec.X, Y * InVec.Y);
}

template<typename T>
inline TVector2<T>& TVector2<T>::operator*=(const TVector2<T>& InVec)
{
	X *= InVec.X;
	Y *= InVec.Y;
	return *this;
}

template<typename T>
inline TVector2<T> TVector2<T>::operator/(const TVector2<T>& InVec) const
{
	return TVector2<T>(X / InVec.X, Y / InVec.Y);
}

template<typename T>
inline TVector2<T>& TVector2<T>::operator/=(const TVector2<T>& InVec)
{
	X /= InVec.X;
	Y /= InVec.Y;
	return *this;
}

template<typename T>
inline TVector2<T> TVector2<T>::operator+(T InScalar) const
{
	return TVector2<T>(X + InScalar, Y + InScalar);
}

template<typename T>
inline TVector2<T>& TVector2<T>::operator+=(T InScalar)
{
	X += InScalar;
	Y += InScalar;
	return *this;
}

template<typename T>
inline TVector2<T> TVector2<T>::operator-(T InScalar) const
{
	return TVector2<T>(X - InScalar, Y - InScalar);
}

template<typename T>
inline TVector2<T>& TVector2<T>::operator-=(T InScalar)
{
	X -= InScalar;
	Y -= InScalar;
	return *this;
}

template<typename T>
inline TVector2<T> TVector2<T>::operator*(T InScalar) const
{
	return TVector2<T>(X * InScalar, Y * InScalar);
}

template<typename T>
inline TVector2<T>& TVector2<T>::operator*=(T InScalar)
{
	X *= InScalar;
	Y *= InScalar;
	return *this;
}

template<typename T>
inline TVector2<T> TVector2<T>::operator/(T InScalar) const
{
	const T InvScalar = 1.0F / InScalar;
	return TVector2<T>(X * InvScalar, Y * InvScalar);
}

template<typename T>
inline TVector2<T>& TVector2<T>::operator/=(T InScalar)
{
	const T InvScalar = 1.0F / InScalar;
	X *= InvScalar;
	Y *= InvScalar;
	return *this;
}

template<typename T>
inline T TVector2<T>::Dot(const TVector2<T>& InVecA, const TVector2<T>& InVecB)
{
	return InVecA.X * InVecB.X + InVecA.Y * InVecB.Y;
}

template<typename T>
inline TVector2<T> TVector2<T>::Cross(const TVector2<T>& InVecA, const TVector2<T>& InVecB)
{
	return TVector2<T>(InVecA.X * InVecB.Y, InVecA.Y * InVecB.X);
}

template<typename T>
inline T TVector2<T>::Length() const
{
	return std::sqrt(X * X + Y * Y);
}

template<typename T>
inline void TVector2<T>::Normalize()
{
	T Scale = Length();
	if (Scale != 0.0F) {
		Scale = 1.0F / Scale;
		X *= Scale;
		Y *= Scale;
	}
}

template<typename T>
inline TVector2<T> TVector2<T>::Normalized() const
{
	TVector2<T> Result = *this;
	Result.Normalize();
	return Result;
}

}
}