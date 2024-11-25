#pragma once
#include "MathUtils.h"
#include <cassert>
#include <cmath>
#include "Matrix4.h"

namespace Rev
{
namespace Math
{

template<typename T>
inline TMatrix4<T>::TMatrix4()
{
}

template<typename T>
inline TMatrix4<T>::TMatrix4(T InScalar)
{
	Columns[0] = TVector4<T>(InScalar, 0.0F, 0.0F, 0.0F);
	Columns[1] = TVector4<T>(0.0F, InScalar, 0.0F, 0.0F);
	Columns[2] = TVector4<T>(0.0F, 0.0F, InScalar, 0.0F);
	Columns[3] = TVector4<T>(0.0F, 0.0F, 0.0F, InScalar);
}

template<typename T>
inline TMatrix4<T>::TMatrix4(TVector4<T> InCol0, TVector4<T> InCol1, TVector4<T> InCol2, TVector4<T> InCol3)
{
	Columns[0] = InCol0;
	Columns[1] = InCol1;
	Columns[2] = InCol2;
	Columns[3] = InCol3;
}

template<typename T>
inline TMatrix4<T>::TMatrix4(TMatrix3<T> InMat)
{
	Columns[0] = InMat[0];
	Columns[1] = InMat[1];
	Columns[2] = InMat[2];
	Columns[3] = TVector4<T>(0.0F, 0.0F, 0.0F, 1.0F);
}

template<typename T>
inline TVector4<T>& TMatrix4<T>::operator[](int Index)
{
	assert((Index >= 0 && Index < 4) && "Matrix4 index out of range");
	return Columns[Index];
}

template<typename T>
inline TVector4<T> const& TMatrix4<T>::operator[](int Index) const
{
	assert((Index >= 0 && Index < 4) && "Matrix4 index out of range");
	return Columns[Index];
}

template<typename T>
inline T const* TMatrix4<T>::Data() const
{
	return &(Columns[0].X);
}

template<typename T>
inline TMatrix4<T>& TMatrix4<T>::operator=(const TMatrix4<T>& InMat)
{
	Columns[0] = InMat[0];
	Columns[1] = InMat[1];
	Columns[2] = InMat[2];
	Columns[3] = InMat[3];
	return *this;
}

template<typename T>
inline TMatrix4<T> TMatrix4<T>::operator+(const TMatrix4<T>& InMat) const
{
	return TMatrix4<T>(Columns[0] + InMat[0], Columns[1] + InMat[1], Columns[2] + InMat[2], Columns[3] + InMat[3]);
}

template<typename T>
inline TMatrix4<T>& TMatrix4<T>::operator+=(const TMatrix4<T>& InMat)
{
	Columns[0] += InMat[0];
	Columns[1] += InMat[1];
	Columns[2] += InMat[2];
	Columns[3] += InMat[3];
	return *this;
}

template<typename T>
inline TMatrix4<T> TMatrix4<T>::operator-(const TMatrix4<T>& InMat) const
{
	return TMatrix4<T>(Columns[0] - InMat[0], Columns[1] - InMat[1], Columns[2] - InMat[2], Columns[3] - InMat[3]);
}

template<typename T>
inline TMatrix4<T>& TMatrix4<T>::operator-=(const TMatrix4<T>& InMat)
{
	Columns[0] -= InMat[0];
	Columns[1] -= InMat[1];
	Columns[2] -= InMat[2];
	Columns[3] -= InMat[3];
	return *this;
}

template<typename T>
inline TMatrix4<T> TMatrix4<T>::operator*(const TMatrix4<T>& InMat) const
{
	//From GLM
	TVector4<T> const SrcA0 = Columns[0];
	TVector4<T> const SrcA1 = Columns[1];
	TVector4<T> const SrcA2 = Columns[2];
	TVector4<T> const SrcA3 = Columns[3];

	TVector4<T> const SrcB0 = InMat[0];
	TVector4<T> const SrcB1 = InMat[1];
	TVector4<T> const SrcB2 = InMat[2];
	TVector4<T> const SrcB3 = InMat[3];

	TMatrix4<T> Result;
	Result[0] = SrcA0 * SrcB0[0] + SrcA1 * SrcB0[1] + SrcA2 * SrcB0[2] + SrcA3 * SrcB0[3];
	Result[1] = SrcA0 * SrcB1[0] + SrcA1 * SrcB1[1] + SrcA2 * SrcB1[2] + SrcA3 * SrcB1[3];
	Result[2] = SrcA0 * SrcB2[0] + SrcA1 * SrcB2[1] + SrcA2 * SrcB2[2] + SrcA3 * SrcB2[3];
	Result[3] = SrcA0 * SrcB3[0] + SrcA1 * SrcB3[1] + SrcA2 * SrcB3[2] + SrcA3 * SrcB3[3];
	return Result;
}

template<typename T>
inline TMatrix4<T>& TMatrix4<T>::operator*=(const TMatrix4<T>& InMat)
{
	return (*this = *this * InMat);
}

template<typename T>
inline TMatrix4<T> TMatrix4<T>::operator*(T InScalar) const
{
	return TMatrix4<T>(Columns[0] * InScalar, Columns[1] * InScalar, Columns[2] * InScalar, Columns[3] * InScalar);
}

template<typename T>
inline TMatrix4<T>& TMatrix4<T>::operator*=(T InScalar)
{
	Columns[0] *= InScalar;
	Columns[1] *= InScalar;
	Columns[2] *= InScalar;
	Columns[3] *= InScalar;
	return *this;
}

template<typename T>
inline TVector4<T> TMatrix4<T>::operator*(const TVector4<T>& InVec) const
{
	TVector4<T> Result;
	Result[0] = Columns[0][0] * InVec[0] + Columns[0][1] * InVec[1] + Columns[0][2] * InVec[2] + Columns[0][3] * InVec[3];
	Result[1] = Columns[1][0] * InVec[0] + Columns[1][1] * InVec[1] + Columns[1][2] * InVec[2] + Columns[1][3] * InVec[3];
	Result[2] = Columns[2][0] * InVec[0] + Columns[2][1] * InVec[1] + Columns[2][2] * InVec[2] + Columns[2][3] * InVec[3];
	Result[3] = Columns[3][0] * InVec[0] + Columns[3][1] * InVec[1] + Columns[3][2] * InVec[2] + Columns[3][3] * InVec[3];
	return Result;
}

template<typename T>
inline void TMatrix4<T>::Invert()
{
	*this = Inverse();
}

template<typename T>
inline TMatrix4<T> TMatrix4<T>::Inverse() const
{
	//From GLM
	T Coef00 = Columns[2][2] * Columns[3][3] - Columns[3][2] * Columns[2][3];
	T Coef02 = Columns[1][2] * Columns[3][3] - Columns[3][2] * Columns[1][3];
	T Coef03 = Columns[1][2] * Columns[2][3] - Columns[2][2] * Columns[1][3];

	T Coef04 = Columns[2][1] * Columns[3][3] - Columns[3][1] * Columns[2][3];
	T Coef06 = Columns[1][1] * Columns[3][3] - Columns[3][1] * Columns[1][3];
	T Coef07 = Columns[1][1] * Columns[2][3] - Columns[2][1] * Columns[1][3];

	T Coef08 = Columns[2][1] * Columns[3][2] - Columns[3][1] * Columns[2][2];
	T Coef10 = Columns[1][1] * Columns[3][2] - Columns[3][1] * Columns[1][2];
	T Coef11 = Columns[1][1] * Columns[2][2] - Columns[2][1] * Columns[1][2];

	T Coef12 = Columns[2][0] * Columns[3][3] - Columns[3][0] * Columns[2][3];
	T Coef14 = Columns[1][0] * Columns[3][3] - Columns[3][0] * Columns[1][3];
	T Coef15 = Columns[1][0] * Columns[2][3] - Columns[2][0] * Columns[1][3];

	T Coef16 = Columns[2][0] * Columns[3][2] - Columns[3][0] * Columns[2][2];
	T Coef18 = Columns[1][0] * Columns[3][2] - Columns[3][0] * Columns[1][2];
	T Coef19 = Columns[1][0] * Columns[2][2] - Columns[2][0] * Columns[1][2];

	T Coef20 = Columns[2][0] * Columns[3][1] - Columns[3][0] * Columns[2][1];
	T Coef22 = Columns[1][0] * Columns[3][1] - Columns[3][0] * Columns[1][1];
	T Coef23 = Columns[1][0] * Columns[2][1] - Columns[2][0] * Columns[1][1];

	TVector4<T> Fac0(Coef00, Coef00, Coef02, Coef03);
	TVector4<T> Fac1(Coef04, Coef04, Coef06, Coef07);
	TVector4<T> Fac2(Coef08, Coef08, Coef10, Coef11);
	TVector4<T> Fac3(Coef12, Coef12, Coef14, Coef15);
	TVector4<T> Fac4(Coef16, Coef16, Coef18, Coef19);
	TVector4<T> Fac5(Coef20, Coef20, Coef22, Coef23);

	TVector4<T> Vec0(Columns[1][0], Columns[0][0], Columns[0][0], Columns[0][0]);
	TVector4<T> Vec1(Columns[1][1], Columns[0][1], Columns[0][1], Columns[0][1]);
	TVector4<T> Vec2(Columns[1][2], Columns[0][2], Columns[0][2], Columns[0][2]);
	TVector4<T> Vec3(Columns[1][3], Columns[0][3], Columns[0][3], Columns[0][3]);

	TVector4<T> Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
	TVector4<T> Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
	TVector4<T> Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
	TVector4<T> Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

	TVector4<T> SignA(+1, -1, +1, -1);
	TVector4<T> SignB(-1, +1, -1, +1);
	TMatrix4<T> Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);

	TVector4<T> Row0(Inverse[0][0], Inverse[1][0], Inverse[2][0], Inverse[3][0]);

	TVector4<T> Dot0(Columns[0] * Row0);
	T Dot1 = (Dot0.X + Dot0.Y) + (Dot0.Z + Dot0.W);

	T OneOverDeterminant = static_cast<T>(1) / Dot1;

	return Inverse * OneOverDeterminant;
}

template<typename T>
inline void TMatrix4<T>::Transpose()
{
	*this = Transposed();
}

template<typename T>
inline TMatrix4<T> TMatrix4<T>::Transposed() const
{
	TMatrix4<T> Transposed;
	Transposed[0][0] = Columns[0][0];
	Transposed[0][1] = Columns[1][0];
	Transposed[0][2] = Columns[2][0];
	Transposed[0][3] = Columns[3][0];
	Transposed[1][0] = Columns[0][1];
	Transposed[1][1] = Columns[1][1];
	Transposed[1][2] = Columns[2][1];
	Transposed[1][3] = Columns[3][1];
	Transposed[2][0] = Columns[0][2];
	Transposed[2][1] = Columns[1][2];
	Transposed[2][2] = Columns[2][2];
	Transposed[2][3] = Columns[3][2];
	Transposed[3][0] = Columns[0][3];
	Transposed[3][1] = Columns[1][3];
	Transposed[3][2] = Columns[2][3];
	Transposed[3][3] = Columns[3][3];
	return Transposed;
}

template<typename T>
inline TVector3<T> TMatrix4<T>::TransfromPosition(const TVector3<T>& InVec) const
{
	return (*this) * TVector4<T>(InVec, 1.0F);
}

template<typename T>
inline TVector3<T> TMatrix4<T>::TransfromVector(const TVector3<T>& InVec) const
{
	return (*this) * TVector4<T>(InVec, 0.0F);
}

template<typename T>
inline TMatrix4<T> TMatrix4<T>::FromTranslate(const TVector3<T>& InTrans)
{
	TMatrix4<T> Result(1.0F);
	Result[3][0] = InTrans.X;
	Result[3][1] = InTrans.Y;
	Result[3][2] = InTrans.Z;
	return Result;
}

template<typename T>
inline TMatrix4<T> TMatrix4<T>::FromEuler(const TRotator<T>& InRot)
{
	T rPitch = Radians(InRot.Pitch);
	T rYaw = Radians(InRot.Yaw);
	T rRoll = Radians(InRot.Roll);

	//From GLM
	TMatrix4<T> Result(T(1.0F));
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
inline TMatrix4<T> TMatrix4<T>::FromQuat(const TQuaternion<T>& InQuat)
{
	//From GLM
	TMatrix4<T> Result(T(1.0F));
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
inline TMatrix4<T> TMatrix4<T>::FromScale(const TVector3<T>& InScale)
{
	constexpr float MinScale = 1.192092896e-07F;

	TMatrix4<T> Result;
	Result[0][0] = InScale.X < MinScale ? MinScale : InScale.X;
	Result[1][1] = InScale.Y < MinScale ? MinScale : InScale.Y;
	Result[2][2] = InScale.Z < MinScale ? MinScale : InScale.Z;
	Result[3][3] = 1.0F;
	return Result;
}

template<typename T>
inline TMatrix4<T> TMatrix4<T>::FromTRS(const TVector3<T>& InT, const TRotator<T>& InR, const TVector3<T>& InS)
{
	return TMatrix4<T>::FromTranslate(InT) * TMatrix4<T>::FromEuler(InR) * TMatrix4<T>::FromScale(InS);
}

template<typename T>
inline TMatrix4<T> TMatrix4<T>::FromTRS(const TVector3<T>& InT, const TQuaternion<T>& InQ, const TVector3<T>& InS)
{
	return TMatrix4<T>::FromTranslate(InT) * TMatrix4<T>::FromQuat(InQ) * TMatrix4<T>::FromScale(InS);
}

template<typename T>
inline TMatrix4<T> TMatrix4<T>::Perspective(float InFOV, float InAspectRatio, float InNear, float InFar)
{
	//From GLM
	assert(std::abs(InAspectRatio) > 0.0F);

	float const tanHalfFovy = std::tan(InFOV / 2.0F);

	TMatrix4<T> Result;
	Result[0][0] = 1.0F / (InAspectRatio * tanHalfFovy);
	Result[1][1] = 1.0F / (tanHalfFovy);
	Result[2][2] = -(InFar + InNear) / (InFar - InNear);
	Result[2][3] = -1.0F;
	Result[3][2] = -(2.0F * InFar * InNear) / (InFar - InNear);
	return Result;
}

template<typename T>
inline TMatrix4<T> TMatrix4<T>::Othographic(float InLeft, float InRight, float InBottom, float InTop, float InNear, float InFar)
{
	//From GLM
	TMatrix4<T> Result(1.0F);
	Result[0][0] = 2.0F / (InRight - InLeft);
	Result[1][1] = 2.0F / (InTop - InBottom);
	Result[3][0] = -(InRight + InLeft) / (InRight - InLeft);
	Result[3][1] = -(InTop + InBottom) / (InTop - InBottom);

	Result[2][2] = -1.0F / (InFar - InNear);
	Result[3][2] = -InNear / (InFar - InNear);

	//Result[2][2] = 2.0F / (InFar - InNear);
	//Result[3][2] = -(InFar + InNear) / (InFar - InNear);

	return Result;
}

template<typename T>
inline TMatrix4<T> TMatrix4<T>::LookAt(const TVector3<T>& InEye, const TVector3<T>& InCenter, const TVector3<T>& InUp)
{
	TVector3<T> const f((InCenter - InEye).Normalized());
	TVector3<T> const s(TVector3<T>::Cross(f, InUp).Normalized());
	TVector3<T> const u(TVector3<T>::Cross(s, f));

	TMatrix4<T> Result(1.0F);
	Result[0][0] = s.x;
	Result[1][0] = s.y;
	Result[2][0] = s.z;
	Result[0][1] = u.x;
	Result[1][1] = u.y;
	Result[2][1] = u.z;
	Result[0][2] = -f.x;
	Result[1][2] = -f.y;
	Result[2][2] = -f.z;
	Result[3][0] = -TVector3<T>::Dot(s, InEye);
	Result[3][1] = -TVector3<T>::Dot(u, InEye);
	Result[3][2] = TVector3<T>::Dot(f, InEye);
	return Result;
}

}
}