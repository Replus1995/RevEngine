#pragma once
#include <type_traits>

namespace Rev
{
namespace Math
{

template<typename T>
struct TVector2
{
	static_assert(std::is_floating_point_v<T>, "T must be floating point");
public:
	T X;
	T Y;

	TVector2();
	TVector2(T InScalar);
	TVector2(T InX, T InY);
	TVector2(const TVector2<T>& InVec);

	T& operator[](int Index);
	T const& operator[](int Index) const;
	T const* Data() const;

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

using FVector2 = TVector2<float>;

}
}

#include "Vector2.inl"