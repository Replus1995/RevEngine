#pragma once
#include "Vector3.h"

namespace Rev
{
namespace Math
{

template<typename T>
struct TVector4
{
	static_assert(std::is_floating_point_v<T>, "T must be floating point");
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

	operator TVector3<T>() const { return  TVector3<T>(X, Y, Z); }
	T& operator[](int Index);
	T const& operator[](int Index) const;
	T const* Data() const;

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

using FVector4 = TVector4<float>;

}
}

#include "Vector4.inl"

