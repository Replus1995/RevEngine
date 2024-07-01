#pragma once
#include "Pine/Core/Base.h"

namespace Pine
{

template<typename T>
struct TVector2
{
public:
	T X;
	T Y;

	TVector2();
	TVector2(T InScalar);
	TVector2(T InX, T InY);
	TVector2(const TVector2<T>& InVec);

	T& operator[](int Index);
	T const& operator[](int Index) const;

	bool operator==(const TVector2<T>& InVec) const;
	bool operator!=(const TVector2<T>& InVec) const;
	TVector2<T> operator-() const;
	TVector2<T>& operator=(const TVector2<T>& InVec);

	TVector2<T> operator+(const TVector2<T>& InVec) const;
	TVector2<T>& operator+=(const TVector2<T>& InVec);
	TVector2<T> operator-(const TVector2<T>& InVec) const;
	TVector2<T>& operator-=(const TVector2<T>& InVec);
	TVector2<T> operator*(const TVector2<T>& InVec) const;
	TVector2<T>& operator*=(const TVector2<T>& InVec);
	TVector2<T> operator/(const TVector2<T>& InVec) const;
	TVector2<T>& operator/=(const TVector2<T>& InVec);

	TVector2 operator+(T InScalar) const;
	TVector2& operator+=(T InScalar);
	TVector2 operator-(T InScalar) const;
	TVector2& operator-=(T InScalar);
	TVector2 operator*(T InScalar) const;
	TVector2& operator*=(T InScalar);
	TVector2 operator/(T InScalar) const;
	TVector2& operator/=(T InScalar);

	static TVector2<T> Cross(const TVector2<T>& InVecA, const TVector2<T>& InVecB);

	T Length() const;
	void Normalize();
	TVector2<T> Normalized() const;
};

template class PINE_API TVector2<float>;
template class PINE_API TVector2<double>;
using FVector2 = TVector2<float>;

}