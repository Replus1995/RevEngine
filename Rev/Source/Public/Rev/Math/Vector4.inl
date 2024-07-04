#pragma once
#include <cassert>
#include <cmath>

namespace Rev
{
namespace Math
{

template<typename T>
inline TVector4<T>::TVector4()
	: X(0.0F)
	, Y(0.0F)
	, Z(0.0F)
	, W(0.0F)
{
}

template<typename T>
inline TVector4<T>::TVector4(T InScalar)
	: X(InScalar)
	, Y(InScalar)
	, Z(InScalar)
	, W(InScalar)
{
}

template<typename T>
inline TVector4<T>::TVector4(T InX, T InY, T InZ, T InW)
	: X(InX)
	, Y(InY)
	, Z(InZ)
	, W(InW)
{
}

template<typename T>
inline TVector4<T>::TVector4(const TVector3<T>& InVec, T InW)
	: X(InVec.X)
	, Y(InVec.Y)
	, Z(InVec.Z)
	, W(InW)
{
}

template<typename T>
inline TVector4<T>::TVector4(const TVector4<T>& InVec)
	: X(InVec.X)
	, Y(InVec.Y)
	, Z(InVec.Z)
	, W(InVec.W)
{
}

template<typename T>
inline T& TVector4<T>::operator[](int Index)
{
	assert((Index >= 0 && Index < 4) && "Vector4 index out of range");
	switch (Index)
	{
	default:
	case 0:
		return X;
	case 1:
		return Y;
	case 2:
		return Z;
	case 3:
		return W;
	}
}

template<typename T>
inline T const& TVector4<T>::operator[](int Index) const
{
	assert((Index >= 0 && Index < 4) && "Vector4 index out of range");
	switch (Index)
	{
	default:
	case 0:
		return X;
	case 1:
		return Y;
	case 2:
		return Z;
	case 3:
		return W;
	}
}

template<typename T>
inline T const* TVector4<T>::Data() const
{
	return &X;
}

template<typename T>
inline bool TVector4<T>::operator==(const TVector4<T>& InVec) const
{
	return X == InVec.X && Y == InVec.Y && Z == InVec.Z && W == InVec.W;
}

template<typename T>
inline bool TVector4<T>::operator!=(const TVector4<T>& InVec) const
{
	return X != InVec.X || Y != InVec.Y || Z != InVec.Z|| W != InVec.W;
}

template<typename T>
inline TVector4<T> TVector4<T>::operator-() const
{
	return TVector4<T>(-X, -Y, -Z, -W);
}

template<typename T>
inline TVector4<T>& TVector4<T>::operator=(const TVector4<T>& InVec)
{
	X = InVec.X;
	Y = InVec.Y;
	Z = InVec.Z;
	W = InVec.W;
	return *this;
}

template<typename T>
inline TVector4<T> TVector4<T>::operator+(const TVector4<T>& InVec) const
{
	return TVector4<T>(X + InVec.X, Y + InVec.Y, Z + InVec.Z, W + InVec.W);
}

template<typename T>
inline TVector4<T>& TVector4<T>::operator+=(const TVector4<T>& InVec)
{
	X += InVec.X;
	Y += InVec.Y;
	Z += InVec.Z;
	W += InVec.W;
	return *this;
}

template<typename T>
inline TVector4<T> TVector4<T>::operator-(const TVector4<T>& InVec) const
{
	return TVector4<T>(X - InVec.X, Y - InVec.Y, Z - InVec.Z, W - InVec.W);
}

template<typename T>
inline TVector4<T>& TVector4<T>::operator-=(const TVector4<T>& InVec)
{
	X -= InVec.X;
	Y -= InVec.Y;
	Z -= InVec.Z;
	W -= InVec.W;
	return *this;
}

template<typename T>
inline TVector4<T> TVector4<T>::operator*(const TVector4<T>& InVec) const
{
	return TVector4<T>(X * InVec.X, Y * InVec.Y, Z * InVec.Z, W * InVec.W);
}

template<typename T>
inline TVector4<T>& TVector4<T>::operator*=(const TVector4<T>& InVec)
{
	X *= InVec.X;
	Y *= InVec.Y;
	Z *= InVec.Z;
	W *= InVec.W;
	return *this;
}

template<typename T>
inline TVector4<T> TVector4<T>::operator/(const TVector4<T>& InVec) const
{
	return TVector4<T>(X / InVec.X, Y / InVec.Y, Z / InVec.Z, W / InVec.W);
}

template<typename T>
inline TVector4<T>& TVector4<T>::operator/=(const TVector4<T>& InVec)
{
	X /= InVec.X;
	Y /= InVec.Y;
	Z /= InVec.Z;
	W /= InVec.W;
	return *this;
}

template<typename T>
inline TVector4<T> TVector4<T>::operator+(T InScalar) const
{
	return TVector4<T>(X + InScalar, Y + InScalar, Z + InScalar, W + InScalar);
}

template<typename T>
inline TVector4<T>& TVector4<T>::operator+=(T InScalar)
{
	X += InScalar;
	Y += InScalar;
	Z += InScalar;
	W += InScalar;
	return *this;
}

template<typename T>
inline TVector4<T> TVector4<T>::operator-(T InScalar) const
{
	return TVector4<T>(X - InScalar, Y - InScalar, Z - InScalar, W - InScalar);
}

template<typename T>
inline TVector4<T>& TVector4<T>::operator-=(T InScalar)
{
	X -= InScalar;
	Y -= InScalar;
	Z -= InScalar;
	W -= InScalar;
	return *this;
}

template<typename T>
inline TVector4<T> TVector4<T>::operator*(T InScalar) const
{
	return TVector4<T>(X * InScalar, Y * InScalar, Z * InScalar, W * InScalar);
}

template<typename T>
inline TVector4<T>& TVector4<T>::operator*=(T InScalar)
{
	X *= InScalar;
	Y *= InScalar;
	Z *= InScalar;
	W *= InScalar;
	return *this;
}

template<typename T>
inline TVector4<T> TVector4<T>::operator/(T InScalar) const
{
	const T InvScalar = 1.0F / InScalar;
	return TVector4<T>(X * InvScalar, Y * InvScalar, Z * InvScalar, W * InvScalar);
}

template<typename T>
inline TVector4<T>& TVector4<T>::operator/=(T InScalar)
{
	const T InvScalar = 1.0F / InScalar;
	X *= InvScalar;
	Y *= InvScalar;
	Z *= InvScalar;
	W *= InvScalar;
	return *this;
}

template<typename T>
inline T TVector4<T>::Length() const
{
	return std::sqrt(X * X + Y * Y + Z * Z + W * W);
}

template<typename T>
void TVector4<T>::Normalize()
{
	T Scale = Length();
	if (Scale != 0.0F) {
		Scale = 1.0F / Scale;
		X *= Scale;
		Y *= Scale;
		Z *= Scale;
		W *= Scale;
	}
}

template<typename T>
inline TVector4<T> TVector4<T>::Normalized() const
{
	TVector4<T> Result = *this;
	Result.Normalize();
	return Result;
}

}
}