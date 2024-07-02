#pragma once
#include <cassert>
#include <cmath>

namespace Rev::Math
{

template<typename T>
TVector3<T>::TVector3()
	: X(0.0F)
	, Y(0.0F)
	, Z(0.0F)
{
}

template<typename T>
TVector3<T>::TVector3(T InScalar)
	: X(InScalar)
	, Y(InScalar)
	, Z(InScalar)
{
}

template<typename T>
TVector3<T>::TVector3(T InX, T InY, T InZ)
	: X(InX)
	, Y(InY)
	, Z(InZ)
{
}

template<typename T>
TVector3<T>::TVector3(const TVector2<T>& InVec, T InZ)
	: X(InVec.X)
	, Y(InVec.Y)
	, Z(InZ)
{
}

template<typename T>
TVector3<T>::TVector3(const TVector3<T>& InVec)
	: X(InVec.X)
	, Y(InVec.Y)
	, Z(InVec.Z)
{
}

template<typename T>
T& TVector3<T>::operator[](int Index)
{
	assert((Index >= 0 && Index < 3) && "Vector3 index out of range");
	switch (Index)
	{
	default:
	case 0:
		return X;
	case 1:
		return Y;
	case 2:
		return Z;
	}
}

template<typename T>
T const& TVector3<T>::operator[](int Index) const
{
	assert((Index >= 0 && Index < 3) && " Vector3 index out of range");
	switch (Index)
	{
	default:
	case 0:
		return X;
	case 1:
		return Y;
	case 2:
		return Z;
	}
}

template<typename T>
bool TVector3<T>::operator==(const TVector3<T>& InVec) const
{
	return X == InVec.X && Y == InVec.Y && Z == InVec.Z;
}

template<typename T>
bool TVector3<T>::operator!=(const TVector3<T>& InVec) const
{
	return X != InVec.X || Y != InVec.Y || Z != InVec.Z;
}

template<typename T>
TVector3<T> TVector3<T>::operator-() const
{
	return TVector3<T>(-X, -Y, -Z);
}

template<typename T>
TVector3<T>& TVector3<T>::operator=(const TVector3<T>& InVec)
{
	X = InVec.X;
	Y = InVec.Y;
	Z = InVec.Z;
	return *this;
}

template<typename T>
TVector3<T> TVector3<T>::operator+(const TVector3<T>& InVec) const
{
	return TVector3(X + InVec.X, Y + InVec.Y, Z + InVec.Z);
}

template<typename T>
TVector3<T>& TVector3<T>::operator+=(const TVector3<T>& InVec)
{
	X += InVec.X;
	Y += InVec.Y;
	Z += InVec.Z;
	return *this;
}

template<typename T>
TVector3<T> TVector3<T>::operator-(const TVector3<T>& InVec) const
{
	return TVector3<T>(X - InVec.X, Y - InVec.Y, Z - InVec.Z);
}

template<typename T>
TVector3<T>& TVector3<T>::operator-=(const TVector3<T>& InVec)
{
	X -= InVec.X;
	Y -= InVec.Y;
	Z -= InVec.Z;
	return *this;
}

template<typename T>
TVector3<T> TVector3<T>::operator*(const TVector3<T>& InVec) const
{
	return TVector3<T>(X * InVec.X, Y * InVec.Y, Z * InVec.Z);
}

template<typename T>
TVector3<T>& TVector3<T>::operator*=(const TVector3<T>& InVec)
{
	X *= InVec.X;
	Y *= InVec.Y;
	Z *= InVec.Z;
	return *this;
}

template<typename T>
TVector3<T> TVector3<T>::operator/(const TVector3<T>& InVec) const
{
	return TVector3<T>(X / InVec.X, Y / InVec.Y, Z / InVec.Z);
}

template<typename T>
TVector3<T>& TVector3<T>::operator/=(const TVector3<T>& InVec)
{
	X /= InVec.X;
	Y /= InVec.Y;
	Z /= InVec.Z;
	return *this;
}

template<typename T>
TVector3<T> TVector3<T>::operator+(T InScalar) const
{
	return TVector3<T>(X + InScalar, Y + InScalar, Z + InScalar);
}

template<typename T>
TVector3<T>& TVector3<T>::operator+=(T InScalar)
{
	X += InScalar;
	Y += InScalar;
	Z += InScalar;
	return *this;
}

template<typename T>
TVector3<T> TVector3<T>::operator-(T InScalar) const
{
	return TVector3<T>(X - InScalar, Y - InScalar, Z - InScalar);
}

template<typename T>
TVector3<T>& TVector3<T>::operator-=(T InScalar)
{
	X -= InScalar;
	Y -= InScalar;
	Z -= InScalar;
	return *this;
}

template<typename T>
TVector3<T> TVector3<T>::operator*(T InScalar) const
{
	return TVector3<T>(X * InScalar, Y * InScalar, Z * InScalar);
}

template<typename T>
TVector3<T>& TVector3<T>::operator*=(T InScalar)
{
	X *= InScalar;
	Y *= InScalar;
	Z *= InScalar;
	return *this;
}

template<typename T>
TVector3<T> TVector3<T>::operator/(T InScalar) const
{
	return TVector3<T>(X / InScalar, Y / InScalar, Z / InScalar);
}

template<typename T>
TVector3<T>& TVector3<T>::operator/=(T InScalar)
{
	X /= InScalar;
	Y /= InScalar;
	Z /= InScalar;
	return *this;
}

template<typename T>
TVector3<T> TVector3<T>::Cross(const TVector3<T>& InVecA, const TVector3<T>& InVecB)
{
	return TVector3<T>(
		InVecA.Y * InVecB.Z - InVecA.Z * InVecB.Y,
		InVecA.Z * InVecB.X - InVecA.X * InVecB.Z,
		InVecA.X * InVecB.Y - InVecA.Y * InVecB.X
	);
}

template<typename T>
T TVector3<T>::Length() const
{
	return std::sqrt(X * X + Y * Y + Z * Z);
}

template<typename T>
void TVector3<T>::Normalize()
{
	T length = Length();
	if (length != 0.0F) {
		length = 1.0F / length;
		X = X * length;
		Y = Y * length;
		Z = Z * length;
	}
}

template<typename T>
TVector3<T> TVector3<T>::Normalized() const
{
	TVector3<T> Result = *this;
	Result.Normalize();
	return Result;
}

}