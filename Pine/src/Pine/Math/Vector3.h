#pragma once
#include "Vector2.h"

namespace Pine
{

template<typename T>
struct TVector3
{
public:
	T X;
	T Y;
	T Z;

	TVector3();
	TVector3(T InScalar);
	TVector3(T InX, T InY, T InZ);
	TVector3(const TVector2<T>& InVec, T InZ = 0.0F);
	TVector3(const TVector3<T>& InVec);

	T& operator[](int Index);
	T const& operator[](int Index) const;

	bool operator==(const TVector3& InVec) const;
	bool operator!=(const TVector3& InVec) const;

	TVector3<T> operator-() const;
	TVector3<T>& operator=(const TVector3<T>& InVec);
	TVector3<T> operator+(const TVector3<T>& InVec) const;
	TVector3<T>& operator+=(const TVector3<T>& InVec);
	TVector3<T> operator-(const TVector3<T>& InVec) const;
	TVector3<T>& operator-=(const TVector3<T>& InVec);
	TVector3<T> operator*(const TVector3<T>& InVec) const;
	TVector3<T>& operator*=(const TVector3<T>& InVec);
	TVector3<T> operator/(const TVector3<T>& InVec) const;
	TVector3<T>& operator/=(const TVector3<T>& InVec);

	TVector3<T> operator+(T InScalar) const;
	TVector3<T>& operator+=(T InScalar);
	TVector3<T> operator-(T InScalar) const;
	TVector3<T>& operator-=(T InScalar);
	TVector3<T> operator*(T InScalar) const;
	TVector3<T>& operator*=(T InScalar);
	TVector3<T> operator/(T InScalar) const;
	TVector3<T>& operator/=(T InScalar);

	static TVector3<T> Cross(const TVector3<T>& InVecA, const TVector3<T>& InVecB);

	T Length() const;
	void Normalize();
	TVector3<T> Normalized() const;
};

template class PINE_API TVector3<float>;
template class PINE_API TVector3<double>;
using FVector3 = TVector3<float>;

}


