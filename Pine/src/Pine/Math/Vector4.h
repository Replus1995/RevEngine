#pragma once
#include "Vector3.h"

namespace Pine
{

template<typename T>
struct TVector4
{
public:
	T X;
	T Y;
	T Z;
	T W;

	TVector4();
	TVector4(T InScalar);
	TVector4(T InX, T InY, T InZ, T InW);
	TVector4(const TVector3<T>& InVec, T InW = 0.0F);
	TVector4(const TVector4<T>& InVec);

	T& operator[](int Index);
	T const& operator[](int Index) const;

	bool operator==(const TVector4<T>& InVec) const;
	bool operator!=(const TVector4<T>& InVec) const;

	TVector4<T> operator-() const;
	TVector4<T>& operator=(const TVector4<T>& InVec);

	TVector4<T> operator+(const TVector4<T>& InVec) const;
	TVector4<T>& operator+=(const TVector4<T>& InVec);
	TVector4<T> operator-(const TVector4<T>& InVec) const;
	TVector4<T>& operator-=(const TVector4<T>& InVec);
	TVector4<T> operator*(const TVector4<T>& InVec) const;
	TVector4<T>& operator*=(const TVector4<T>& InVec);
	TVector4<T> operator/(const TVector4<T>& InVec) const;
	TVector4<T>& operator/=(const TVector4<T>& InVec);

	TVector4<T> operator+(T InScalar) const;
	TVector4<T>& operator+=(T InScalar);
	TVector4<T> operator-(T InScalar) const;
	TVector4<T>& operator-=(T InScalar);
	TVector4<T> operator*(T InScalar) const;
	TVector4<T>& operator*=(T InScalar);
	TVector4<T> operator/(T InScalar) const;
	TVector4<T>& operator/=(T InScalar);

	T Length() const;
	void Normalize();
	TVector4<T> Normalized() const;
};

template class PINE_API TVector4<float>;
template class PINE_API TVector4<double>;
using FVector4 = TVector4<float>;

}

