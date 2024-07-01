#include "pinepch.h"
#include "Matrix4.h"
#include "Maths.h"
#include <cassert>
#include <cmath>

namespace Pine
{

FMatrix4::FMatrix4()
{
}

FMatrix4::FMatrix4(float InScalar)
{
	Columns[0] = TVector4(InScalar, 0.0F, 0.0F, 0.0F);
	Columns[1] = TVector4(0.0F, InScalar, 0.0F, 0.0F);
	Columns[2] = TVector4(0.0F, 0.0F, InScalar, 0.0F);
	Columns[3] = TVector4(0.0F, 0.0F, 0.0F, InScalar);
}

FVector4& FMatrix4::operator[](int Index)
{
	assert((Index >= 0 && Index < 4) && "Matrix4 index out of range");
	return Columns[Index];
}

FVector4 const& FMatrix4::operator[](int Index) const
{
	assert((Index >= 0 && Index < 4) && "Matrix4 index out of range");
	return Columns[Index];
}

float const* FMatrix4::DataPtr() const
{
	return &(Columns[0].X);
}

FMatrix4 FMatrix4::operator*(const FMatrix4& InMat) const
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

FMatrix4& FMatrix4::operator*=(const FMatrix4& InMat)
{
	return (*this = *this * InMat);
}

FMatrix4 FMatrix4::FromTranslate(const FVector3& InTrans)
{
	FMatrix4 Result(1.0F);
	Result[3][0] = InTrans.X;
	Result[3][1] = InTrans.Y;
	Result[3][2] = InTrans.Z;
	return Result;
}

FMatrix4 FMatrix4::FromEuler(const FRotator& InRot)
{
	float rPitch = FMaths::Radians(InRot.Pitch);
	float rYaw = FMaths::Radians(InRot.Yaw);
	float rRoll = FMaths::Radians(InRot.Roll);

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

FMatrix4 FMatrix4::FromQuat(const FQuaternion& InQuat)
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

FMatrix4 FMatrix4::FromScale(const FVector3& InScale)
{
	FMatrix4 Result;
	Result[0][0] = InScale.X;
	Result[1][1] = InScale.Y;
	Result[2][2] = InScale.Z;
	Result[3][3] = 1.0F;
	return Result;
}

FMatrix4 FMatrix4::FromTRS(const FVector3& InT, const FRotator& InR, const FVector3& InS)
{
	return FMatrix4::FromTranslate(InT) * FMatrix4::FromEuler(InR) * FMatrix4::FromScale(InS);
}

FMatrix4 FMatrix4::FromTRS(const FVector3& InT, const FQuaternion& InQ, const FVector3& InS)
{
	return FMatrix4::FromTranslate(InT) * FMatrix4::FromQuat(InQ) * FMatrix4::FromScale(InS);
}

}