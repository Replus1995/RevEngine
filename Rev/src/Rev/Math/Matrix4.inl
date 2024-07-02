#pragma once
#include "MathUtils.h"
#include <cassert>
#include <cmath>

namespace Rev::Math
{

inline FMatrix4::FMatrix4()
{
}

inline FMatrix4::FMatrix4(float InScalar)
{
	Columns[0] = FVector4(InScalar, 0.0F, 0.0F, 0.0F);
	Columns[1] = FVector4(0.0F, InScalar, 0.0F, 0.0F);
	Columns[2] = FVector4(0.0F, 0.0F, InScalar, 0.0F);
	Columns[3] = FVector4(0.0F, 0.0F, 0.0F, InScalar);
}

inline FMatrix4::FMatrix4(FVector4 InCol0, FVector4 InCol1, FVector4 InCol2, FVector4 InCol3)
{
	Columns[0] = InCol0;
	Columns[1] = InCol1;
	Columns[2] = InCol2;
	Columns[3] = InCol3;
}

inline FMatrix4::FMatrix4(FMatrix3 InMat)
{
	Columns[0] = InMat[0];
	Columns[1] = InMat[1];
	Columns[2] = InMat[2];
	Columns[3] = FVector4(0.0F, 0.0F, 0.0F, 1.0F);
}

inline FVector4& FMatrix4::operator[](int Index)
{
	assert((Index >= 0 && Index < 4) && "Matrix4 index out of range");
	return Columns[Index];
}

inline FVector4 const& FMatrix4::operator[](int Index) const
{
	assert((Index >= 0 && Index < 4) && "Matrix4 index out of range");
	return Columns[Index];
}

inline float const* FMatrix4::DataPtr() const
{
	return &(Columns[0].X);
}

inline FMatrix4 FMatrix4::operator*(const FMatrix4& InMat) const
{
	//From GLM
	FVector4 const SrcA0 = Columns[0];
	FVector4 const SrcA1 = Columns[1];
	FVector4 const SrcA2 = Columns[2];
	FVector4 const SrcA3 = Columns[3];

	FVector4 const SrcB0 = InMat[0];
	FVector4 const SrcB1 = InMat[1];
	FVector4 const SrcB2 = InMat[2];
	FVector4 const SrcB3 = InMat[3];

	FMatrix4 Result;
	Result[0] = SrcA0 * SrcB0[0] + SrcA1 * SrcB0[1] + SrcA2 * SrcB0[2] + SrcA3 * SrcB0[3];
	Result[1] = SrcA0 * SrcB1[0] + SrcA1 * SrcB1[1] + SrcA2 * SrcB1[2] + SrcA3 * SrcB1[3];
	Result[2] = SrcA0 * SrcB2[0] + SrcA1 * SrcB2[1] + SrcA2 * SrcB2[2] + SrcA3 * SrcB2[3];
	Result[3] = SrcA0 * SrcB3[0] + SrcA1 * SrcB3[1] + SrcA2 * SrcB3[2] + SrcA3 * SrcB3[3];
	return Result;
}

inline FMatrix4& FMatrix4::operator*=(const FMatrix4& InMat)
{
	return (*this = *this * InMat);
}

inline FMatrix4 FMatrix4::Inverse() const
{
	//From GLM
	using T = float;
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

	FVector4 Fac0(Coef00, Coef00, Coef02, Coef03);
	FVector4 Fac1(Coef04, Coef04, Coef06, Coef07);
	FVector4 Fac2(Coef08, Coef08, Coef10, Coef11);
	FVector4 Fac3(Coef12, Coef12, Coef14, Coef15);
	FVector4 Fac4(Coef16, Coef16, Coef18, Coef19);
	FVector4 Fac5(Coef20, Coef20, Coef22, Coef23);

	FVector4 Vec0(Columns[1][0], Columns[0][0], Columns[0][0], Columns[0][0]);
	FVector4 Vec1(Columns[1][1], Columns[0][1], Columns[0][1], Columns[0][1]);
	FVector4 Vec2(Columns[1][2], Columns[0][2], Columns[0][2], Columns[0][2]);
	FVector4 Vec3(Columns[1][3], Columns[0][3], Columns[0][3], Columns[0][3]);

	FVector4 Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
	FVector4 Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
	FVector4 Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
	FVector4 Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

	FVector4 SignA(+1, -1, +1, -1);
	FVector4 SignB(-1, +1, -1, +1);
	FMatrix4 Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);

	FVector4 Row0(Inverse[0][0], Inverse[1][0], Inverse[2][0], Inverse[3][0]);

	FVector4 Dot0(Columns[0] * Row0);
	T Dot1 = (Dot0.X + Dot0.Y) + (Dot0.Z + Dot0.W);

	T OneOverDeterminant = static_cast<T>(1) / Dot1;

	return Inverse * OneOverDeterminant;
}

inline FMatrix4 FMatrix4::FromTranslate(const FVector3& InTrans)
{
	FMatrix4 Result(1.0F);
	Result[3][0] = InTrans.X;
	Result[3][1] = InTrans.Y;
	Result[3][2] = InTrans.Z;
	return Result;
}

inline FMatrix4 FMatrix4::FromEuler(const FRotator& InRot)
{
	float rPitch = Radians(InRot.Pitch);
	float rYaw = Radians(InRot.Yaw);
	float rRoll = Radians(InRot.Roll);

	//From GLM
	using T = float;
	FMatrix4 Result(T(1.0F));
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

inline FMatrix4 FMatrix4::FromQuat(const FQuaternion& InQuat)
{
	//From GLM
	using T = float;
	FMatrix4 Result(T(1.0F));
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

inline FMatrix4 FMatrix4::FromScale(const FVector3& InScale)
{
	FMatrix4 Result;
	Result[0][0] = InScale.X;
	Result[1][1] = InScale.Y;
	Result[2][2] = InScale.Z;
	Result[3][3] = 1.0F;
	return Result;
}

inline FMatrix4 FMatrix4::FromTRS(const FVector3& InT, const FRotator& InR, const FVector3& InS)
{
	return FMatrix4::FromTranslate(InT) * FMatrix4::FromEuler(InR) * FMatrix4::FromScale(InS);
}

inline FMatrix4 FMatrix4::FromTRS(const FVector3& InT, const FQuaternion& InQ, const FVector3& InS)
{
	return FMatrix4::FromTranslate(InT) * FMatrix4::FromQuat(InQ) * FMatrix4::FromScale(InS);
}

inline FMatrix4 FMatrix4::Perspective(float InFOV, float InAspectRatio, float InNear, float InFar)
{
	//From GLM
	assert(std::abs(InAspectRatio) > 0.0F);

	float const tanHalfFovy = std::tan(InFOV / 2.0F);

	FMatrix4 Result;
	Result[0][0] = 1.0F / (InAspectRatio * tanHalfFovy);
	Result[1][1] = 1.0F / (tanHalfFovy);
	Result[2][2] = -(InFar + InNear) / (InFar - InNear);
	Result[2][3] = -1.0F;
	Result[3][2] = -(2.0F * InFar * InNear) / (InFar - InNear);
	return Result;
}

inline FMatrix4 FMatrix4::Othographic(float InLeft, float InRight, float InBottom, float InTop, float InNear, float InFar)
{
	//From GLM
	FMatrix4 Result(1.0F);
	Result[0][0] = 2.0F / (InRight - InLeft);
	Result[1][1] = 2.0F / (InTop - InBottom);
	Result[2][2] = 2.0F / (InFar - InNear);
	Result[3][0] = -(InRight + InLeft) / (InRight - InLeft);
	Result[3][1] = -(InTop + InBottom) / (InTop - InBottom);
	Result[3][2] = -(InFar + InNear) / (InFar - InNear);
	return Result;
}

}