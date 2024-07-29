#pragma once
#include "Vector2.h"
#include "Rotator.h"
#include "Quaternion.h"

namespace Rev
{
namespace Math
{

template<typename T>
struct TMatrix2
{
	static_assert(std::is_floating_point_v<T>, "T must be floating point");
public:
	TVector2<T> Columns[2];

	TMatrix2();
	TMatrix2(T InScalar);
	TMatrix2(TVector2<T> InCol0, TVector2<T> InCol1);

	TVector2<T>& operator[](int Index);
	TVector2<T> const& operator[](int Index) const;
	T const* Data() const;

	TMatrix2<T>& operator=(const TMatrix2<T>& InMat);

	TMatrix2<T> operator+(const TMatrix2<T>& InMat) const;
	TMatrix2<T>& operator+=(const TMatrix2<T>& InMat);
	TMatrix2<T> operator-(const TMatrix2<T>& InMat) const;
	TMatrix2<T>& operator-=(const TMatrix2<T>& InMat);
	TMatrix2<T> operator*(const TMatrix2<T>& InMat) const;
	TMatrix2<T>& operator*=(const TMatrix2<T>& InMat);

	TMatrix2<T> operator*(T InScalar) const;
	TMatrix2<T>& operator*=(T InScalar);
	TVector2<T> operator*(const TVector2<T>& InVec) const;

	void Invert();
	TMatrix2<T> Inverse() const;
	void Transpose();
	TMatrix2<T> Transposed() const;
};

using FMatrix2 = TMatrix2<float>;

}
}

#include "Matrix2.inl"