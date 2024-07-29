#pragma once
#include "Vector3.h"
#include "Rotator.h"
#include "Quaternion.h"

namespace Rev
{
namespace Math
{

template<typename T>
struct TMatrix3
{
	static_assert(std::is_floating_point_v<T>, "T must be floating point");
public:
	TVector3<T> Columns[3];

	TMatrix3();
	TMatrix3(T InScalar);
	TMatrix3(TVector3<T> InCol0, TVector3<T> InCol1, TVector3<T> InCol2);

	TVector3<T>& operator[](int Index);
	TVector3<T> const& operator[](int Index) const;
	T const* Data() const;

	TMatrix3<T>& operator=(const TMatrix3<T>& InMat);

	TMatrix3<T> operator+(const TMatrix3<T>& InMat) const;
	TMatrix3<T>& operator+=(const TMatrix3<T>& InMat);
	TMatrix3<T> operator-(const TMatrix3<T>& InMat) const;
	TMatrix3<T>& operator-=(const TMatrix3<T>& InMat);
	TMatrix3<T> operator*(const TMatrix3<T>& InMat) const;
	TMatrix3<T>& operator*=(const TMatrix3<T>& InMat);

	TMatrix3<T> operator*(T InScalar) const;
	TMatrix3<T>& operator*=(T InScalar);
	TVector3<T> operator*(const TVector3<T>& InVec) const;

	void Invert();
	TMatrix3<T> Inverse() const;
	void Transpose();
	TMatrix3<T> Transposed() const;

	static TMatrix3<T> FromEuler(const TRotator<T>& InRot);
	static TMatrix3<T> FromQuat(const TQuaternion<T>& InQuat);

	static TRotator<T> ToEuler(const TMatrix3<T>& InMat);
	static TQuaternion<T> ToQuat(const TMatrix3<T>& InMat);
};

using FMatrix3 = TMatrix3<float>;

}
}

#include "Matrix3.inl"