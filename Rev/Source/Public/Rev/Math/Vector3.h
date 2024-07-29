#pragma once
#include "Vector2.h"

namespace Rev
{
namespace Math
{

template<typename T>
struct TVector3
{
	static_assert(std::is_floating_point_v<T>, "T must be floating point");
public:
	T X;
	T Y;
	T Z;

	TVector3();
	TVector3(T InScalar);
	TVector3(T InX, T InY, T InZ);
	TVector3(const TVector2<T>& InVec, T InZ = 0.0F);
	TVector3(const TVector3<T>& InVec);

	operator TVector2<T>() const { return  TVector2<T>(X, Y); }
	T& operator[](int Index);
	T const& operator[](int Index) const;
	T const* Data() const;

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


	static T Dot(const TVector3<T>& InVecA, const TVector3<T>& InVecB);
	static TVector3<T> Cross(const TVector3<T>& InVecA, const TVector3<T>& InVecB);

	T Length() const;
	void Normalize();
	TVector3<T> Normalized() const;
};

using FVector3 = TVector3<float>;

}
}

#include "Vector3.inl"


