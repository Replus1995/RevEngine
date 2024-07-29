#pragma once
#include "Matrix3.h"
#include "Vector4.h"

namespace Rev
{
namespace Math
{

template<typename T>
struct TMatrix4
{
	static_assert(std::is_floating_point_v<T>, "T must be floating point");
public:
	TVector4<T> Columns[4];

	TMatrix4();
	TMatrix4(T InScalar);
	TMatrix4(TVector4<T> InCol0, TVector4<T> InCol1, TVector4<T> InCol2, TVector4<T> InCol3);
	TMatrix4(TMatrix3<T> InMat);

	operator TMatrix3<T>() const { return  TMatrix3<T>(Columns[0], Columns[1], Columns[2]); }
	TVector4<T>& operator[](int Index);
	TVector4<T> const& operator[](int Index) const;
	T const* Data() const;

	TMatrix4<T>& operator=(const TMatrix4<T>& InMat);

	TMatrix4<T> operator+(const TMatrix4<T>& InMat) const;
	TMatrix4<T>& operator+=(const TMatrix4<T>& InMat);
	TMatrix4<T> operator-(const TMatrix4<T>& InMat) const;
	TMatrix4<T>& operator-=(const TMatrix4<T>& InMat);
	TMatrix4<T> operator*(const TMatrix4<T>& InMat) const;
	TMatrix4<T>& operator*=(const TMatrix4<T>& InMat);

	TMatrix4<T> operator*(T InScalar) const;
	TMatrix4<T>& operator*=(T InScalar);
	TVector4<T> operator*(const TVector4<T>& InVec) const;

	void Invert();
	TMatrix4<T> Inverse() const;
	void Transpose();
	TMatrix4<T> Transposed() const;

	TVector3<T> TransfromPosition(const TVector3<T>& InVec) const;
	TVector3<T> TransfromVector(const TVector3<T>& InVec) const; //No Translation

	static TMatrix4<T> FromTranslate(const TVector3<T>& InTrans);
	static TMatrix4<T> FromEuler(const TRotator<T>& InRot);
	static TMatrix4<T> FromQuat(const TQuaternion<T>& InQuat);
	static TMatrix4<T> FromScale(const TVector3<T>& InScale);

	static TMatrix4<T> FromTRS(const TVector3<T>& InT, const TRotator<T>& InR, const TVector3<T>& InS);
	static TMatrix4<T> FromTRS(const TVector3<T>& InT, const TQuaternion<T>& InQ, const TVector3<T>& InS);

	static TMatrix4<T> Perspective(float InFOV, float InAspectRatio, float InNear, float InFar);
	static TMatrix4<T> Othographic(float InLeft, float InRight, float InBottom, float InTop, float InNear, float InFar);
};

using FMatrix4 = TMatrix4<float>;

}
}

#include "Matrix4.inl"


