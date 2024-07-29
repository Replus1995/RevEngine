#pragma once
#include "MathUtils.h"
#include <cassert>
#include <cmath>

namespace Rev
{
namespace Math
{

template<typename T>
inline TMatrix3<T>::TMatrix3()
{
}

template<typename T>
inline TMatrix3<T>::TMatrix3(T InScalar)
{
	Columns[0] = TVector3<T>(InScalar, 0.0F, 0.0F);
	Columns[1] = TVector3<T>(0.0F, InScalar, 0.0F);
	Columns[2] = TVector3<T>(0.0F, 0.0F, InScalar);
}

template<typename T>
inline TMatrix3<T>::TMatrix3(TVector3<T> InCol0, TVector3<T> InCol1, TVector3<T> InCol2)
{
	Columns[0] = InCol0;
	Columns[1] = InCol1;
	Columns[2] = InCol2;
}

template<typename T>
inline TVector3<T>& TMatrix3<T>::operator[](int Index)
{
	assert((Index >= 0 && Index < 3) && "Matrix3 index out of range");
	return Columns[Index];
}

template<typename T>
inline TVector3<T> const& TMatrix3<T>::operator[](int Index) const
{
	assert((Index >= 0 && Index < 3) && "Matrix3 index out of range");
	return Columns[Index];
}

template<typename T>
inline T const* TMatrix3<T>::Data() const
{
	return &(Columns[0].X);
}

template<typename T>
inline TMatrix3<T>& TMatrix3<T>::operator=(const TMatrix3<T>& InMat)
{
	Columns[0] = InMat[0];
	Columns[1] = InMat[1];
	Columns[2] = InMat[2];
	return *this;
}

template<typename T>
inline TMatrix3<T> TMatrix3<T>::operator+(const TMatrix3<T>& InMat) const
{
	return TMatrix3<T>(Columns[0] + InMat[0], Columns[1] + InMat[1], Columns[2] + InMat[2]);
}

template<typename T>
inline TMatrix3<T>& TMatrix3<T>::operator+=(const TMatrix3<T>& InMat)
{
	Columns[0] += InMat[0];
	Columns[1] += InMat[1];
	Columns[2] += InMat[2];
	return *this;
}

template<typename T>
inline TMatrix3<T> TMatrix3<T>::operator-(const TMatrix3<T>& InMat) const
{
	return TMatrix3<T>(Columns[0] - InMat[0], Columns[1] - InMat[1], Columns[2] - InMat[2]);
}

template<typename T>
inline TMatrix3<T>& TMatrix3<T>::operator-=(const TMatrix3<T>& InMat)
{
	Columns[0] -= InMat[0];
	Columns[1] -= InMat[1];
	Columns[2] -= InMat[2];
	return *this;
}

template<typename T>
inline TMatrix3<T> TMatrix3<T>::operator*(const TMatrix3<T>& InMat) const
{
	TMatrix3<T> const& m1 = *this;
	TMatrix3<T> const& m2 = InMat;

	//From GLM
	T const SrcA00 = m1[0][0];
	T const SrcA01 = m1[0][1];
	T const SrcA02 = m1[0][2];
	T const SrcA10 = m1[1][0];
	T const SrcA11 = m1[1][1];
	T const SrcA12 = m1[1][2];
	T const SrcA20 = m1[2][0];
	T const SrcA21 = m1[2][1];
	T const SrcA22 = m1[2][2];

	T const SrcB00 = m2[0][0];
	T const SrcB01 = m2[0][1];
	T const SrcB02 = m2[0][2];
	T const SrcB10 = m2[1][0];
	T const SrcB11 = m2[1][1];
	T const SrcB12 = m2[1][2];
	T const SrcB20 = m2[2][0];
	T const SrcB21 = m2[2][1];
	T const SrcB22 = m2[2][2];

	TMatrix3<T> Result;
	Result[0][0] = SrcA00 * SrcB00 + SrcA10 * SrcB01 + SrcA20 * SrcB02;
	Result[0][1] = SrcA01 * SrcB00 + SrcA11 * SrcB01 + SrcA21 * SrcB02;
	Result[0][2] = SrcA02 * SrcB00 + SrcA12 * SrcB01 + SrcA22 * SrcB02;
	Result[1][0] = SrcA00 * SrcB10 + SrcA10 * SrcB11 + SrcA20 * SrcB12;
	Result[1][1] = SrcA01 * SrcB10 + SrcA11 * SrcB11 + SrcA21 * SrcB12;
	Result[1][2] = SrcA02 * SrcB10 + SrcA12 * SrcB11 + SrcA22 * SrcB12;
	Result[2][0] = SrcA00 * SrcB20 + SrcA10 * SrcB21 + SrcA20 * SrcB22;
	Result[2][1] = SrcA01 * SrcB20 + SrcA11 * SrcB21 + SrcA21 * SrcB22;
	Result[2][2] = SrcA02 * SrcB20 + SrcA12 * SrcB21 + SrcA22 * SrcB22;
	return Result;
}

template<typename T>
inline TMatrix3<T>& TMatrix3<T>::operator*=(const TMatrix3<T>& InMat)
{
	return (*this = *this * InMat);
}

template<typename T>
inline TMatrix3<T> TMatrix3<T>::operator*(T InScalar) const
{
	return TMatrix3<T>(Columns[0] * InScalar, Columns[1] * InScalar, Columns[2] * InScalar);
}

template<typename T>
inline TMatrix3<T>& TMatrix3<T>::operator*=(T InScalar)
{
	Columns[0] *= InScalar;
	Columns[1] *= InScalar;
	Columns[2] *= InScalar;
	return *this;
}

template<typename T>
inline TVector3<T> TMatrix3<T>::operator*(const TVector3<T>& InVec) const
{
	TVector3<T> Result;
	Result[0] = Columns[0][0] * InVec[0] + Columns[0][1] * InVec[1] + Columns[0][2] * InVec[2];
	Result[1] = Columns[1][0] * InVec[0] + Columns[1][1] * InVec[1] + Columns[1][2] * InVec[2];
	Result[2] = Columns[2][0] * InVec[0] + Columns[2][1] * InVec[1] + Columns[2][2] * InVec[2];
	return Result;
}

template<typename T>
inline void TMatrix3<T>::Invert()
{
	*this = Inverse();
}

template<typename T>
inline TMatrix3<T> TMatrix3<T>::Inverse() const
{
	//From GLM
	T OneOverDeterminant = 1.0F / (
		+ Columns[0][0] * (Columns[1][1] * Columns[2][2] - Columns[2][1] * Columns[1][2])
		- Columns[1][0] * (Columns[0][1] * Columns[2][2] - Columns[2][1] * Columns[0][2])
		+ Columns[2][0] * (Columns[0][1] * Columns[1][2] - Columns[1][1] * Columns[0][2]));

	TMatrix3<T> Inverse;
	Inverse[0][0] = +(Columns[1][1] * Columns[2][2] - Columns[2][1] * Columns[1][2]) * OneOverDeterminant;
	Inverse[1][0] = -(Columns[1][0] * Columns[2][2] - Columns[2][0] * Columns[1][2]) * OneOverDeterminant;
	Inverse[2][0] = +(Columns[1][0] * Columns[2][1] - Columns[2][0] * Columns[1][1]) * OneOverDeterminant;
	Inverse[0][1] = -(Columns[0][1] * Columns[2][2] - Columns[2][1] * Columns[0][2]) * OneOverDeterminant;
	Inverse[1][1] = +(Columns[0][0] * Columns[2][2] - Columns[2][0] * Columns[0][2]) * OneOverDeterminant;
	Inverse[2][1] = -(Columns[0][0] * Columns[2][1] - Columns[2][0] * Columns[0][1]) * OneOverDeterminant;
	Inverse[0][2] = +(Columns[0][1] * Columns[1][2] - Columns[1][1] * Columns[0][2]) * OneOverDeterminant;
	Inverse[1][2] = -(Columns[0][0] * Columns[1][2] - Columns[1][0] * Columns[0][2]) * OneOverDeterminant;
	Inverse[2][2] = +(Columns[0][0] * Columns[1][1] - Columns[1][0] * Columns[0][1]) * OneOverDeterminant;

	return Inverse;
}

template<typename T>
inline void TMatrix3<T>::Transpose()
{
	*this = Transposed();
}

template<typename T>
inline TMatrix3<T> TMatrix3<T>::Transposed() const
{
	TMatrix3<T> Transposed;
	Transposed[0][0] = Columns[0][0];
	Transposed[0][1] = Columns[1][0];
	Transposed[0][2] = Columns[2][0];
	Transposed[1][0] = Columns[0][1];
	Transposed[1][1] = Columns[1][1];
	Transposed[1][2] = Columns[2][1];
	Transposed[2][0] = Columns[0][2];
	Transposed[2][1] = Columns[1][2];
	Transposed[2][2] = Columns[2][2];
	return Transposed;
}

template<typename T>
inline TMatrix3<T> TMatrix3<T>::FromEuler(const TRotator<T>& InRot)
{
	T rPitch = Radians(InRot.Pitch);
	T rYaw = Radians(InRot.Yaw);
	T rRoll = Radians(InRot.Roll);

	//From GLM
	TMatrix3<T> Result(T(1.0F));
	T tmp_ch = std::cos(rYaw);
	T tmp_sh = std::sin(rYaw);
	T tmp_cp = std::cos(rPitch);
	T tmp_sp = std::sin(rPitch);
	T tmp_cb = std::cos(rRoll);
	T tmp_sb = std::sin(rRoll);

	Result[0][0] = tmp_ch * tmp_cb + tmp_sh * tmp_sp * tmp_sb;
	Result[0][1] = tmp_sb * tmp_cp;
	Result[0][2] = -tmp_sh * tmp_cb + tmp_ch * tmp_sp * tmp_sb;

	Result[1][0] = -tmp_ch * tmp_sb + tmp_sh * tmp_sp * tmp_cb;
	Result[1][1] = tmp_cb * tmp_cp;
	Result[1][2] = tmp_sb * tmp_sh + tmp_ch * tmp_sp * tmp_cb;

	Result[2][0] = tmp_sh * tmp_cp;
	Result[2][1] = -tmp_sp;
	Result[2][2] = tmp_ch * tmp_cp;
	return Result;
}

template<typename T>
inline TMatrix3<T> TMatrix3<T>::FromQuat(const TQuaternion<T>& InQuat)
{
	//From GLM
	TMatrix3<T> Result(T(1.0F));
	T qxx(InQuat.X * InQuat.X);
	T qyy(InQuat.Y * InQuat.Y);
	T qzz(InQuat.Z * InQuat.Z);
	T qxz(InQuat.X * InQuat.Z);
	T qxy(InQuat.X * InQuat.Y);
	T qyz(InQuat.Y * InQuat.Z);
	T qwx(InQuat.W * InQuat.X);
	T qwy(InQuat.W * InQuat.Y);
	T qwz(InQuat.W * InQuat.Z);

	Result[0][0] = T(1) - T(2) * (qyy + qzz);
	Result[0][1] = T(2) * (qxy + qwz);
	Result[0][2] = T(2) * (qxz - qwy);

	Result[1][0] = T(2) * (qxy - qwz);
	Result[1][1] = T(1) - T(2) * (qxx + qzz);
	Result[1][2] = T(2) * (qyz + qwx);

	Result[2][0] = T(2) * (qxz + qwy);
	Result[2][1] = T(2) * (qyz - qwx);
	Result[2][2] = T(1) - T(2) * (qxx + qyy);
	return Result;
}

template<typename T>
inline TRotator<T> TMatrix3<T>::ToEuler(const TMatrix3<T>& InMat)
{
	//From GLM
	T T1 = std::atan2(InMat[2][0], InMat[2][2]);
	T C2 = std::sqrt(InMat[0][1] * InMat[0][1] + InMat[1][1] * InMat[1][1]);
	T T2 = std::atan2(-InMat[2][1], C2);
	T S1 = std::sin(T1);
	T C1 = std::cos(T1);
	T T3 = std::atan2(S1 * InMat[1][2] - C1 * InMat[1][0], C1 * InMat[0][0] - S1 * InMat[0][2]);
	return FRotator(Degrees(T2), Degrees(T1), Degrees(T3));
}

template<typename T>
inline TQuaternion<T> TMatrix3<T>::ToQuat(const TMatrix3<T>& InMat)
{
	//From GLM
	TMatrix3<T> const& m = InMat;
	T fourXSquaredMinus1 = m[0][0] - m[1][1] - m[2][2];
	T fourYSquaredMinus1 = m[1][1] - m[0][0] - m[2][2];
	T fourZSquaredMinus1 = m[2][2] - m[0][0] - m[1][1];
	T fourWSquaredMinus1 = m[0][0] + m[1][1] + m[2][2];

	int biggestIndex = 0;
	T fourBiggestSquaredMinus1 = fourWSquaredMinus1;
	if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourXSquaredMinus1;
		biggestIndex = 1;
	}
	if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourYSquaredMinus1;
		biggestIndex = 2;
	}
	if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourZSquaredMinus1;
		biggestIndex = 3;
	}

	T biggestVal = sqrt(fourBiggestSquaredMinus1 + static_cast<T>(1)) * static_cast<T>(0.5);
	T mult = static_cast<T>(0.25) / biggestVal;

	switch (biggestIndex)
	{
	case 0:
		return FQuaternion::WXYZ(biggestVal, (m[1][2] - m[2][1]) * mult, (m[2][0] - m[0][2]) * mult, (m[0][1] - m[1][0]) * mult);
	case 1:
		return FQuaternion::WXYZ((m[1][2] - m[2][1]) * mult, biggestVal, (m[0][1] + m[1][0]) * mult, (m[2][0] + m[0][2]) * mult);
	case 2:
		return FQuaternion::WXYZ((m[2][0] - m[0][2]) * mult, (m[0][1] + m[1][0]) * mult, biggestVal, (m[1][2] + m[2][1]) * mult);
	case 3:
		return FQuaternion::WXYZ((m[0][1] - m[1][0]) * mult, (m[2][0] + m[0][2]) * mult, (m[1][2] + m[2][1]) * mult, biggestVal);
	default: // Silence a -Wswitch-default warning in GCC. Should never actually get here. Assert is just for sanity.
		assert(false);
	}

	return FQuaternion::WXYZ(1, 0, 0, 0);
}

}
}