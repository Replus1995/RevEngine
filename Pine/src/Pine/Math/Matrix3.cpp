#include "pinepch.h"
#include "Matrix3.h"
#include "Maths.h"
#include <cmath>
#include <cassert>

namespace Pine
{

FMatrix3::FMatrix3()
{
}

FMatrix3::FMatrix3(float InScalar)
{
	Columns[0] = TVector3(InScalar, 0.0F, 0.0F);
	Columns[1] = TVector3(0.0F, InScalar, 0.0F);
	Columns[2] = TVector3(0.0F, 0.0F, InScalar);
}

FVector3& FMatrix3::operator[](int Index)
{
	assert((Index >= 0 && Index < 3) && "Matrix3 index out of range");
	return Columns[Index];
}

FVector3 const& FMatrix3::operator[](int Index) const
{
	assert((Index >= 0 && Index < 3) && "Matrix3 index out of range");
	return Columns[Index];
}

float const* FMatrix3::DataPtr() const
{
	return &(Columns[0].X);
}

FMatrix3 FMatrix3::operator*(const FMatrix3& InMat) const
{
	const FMatrix3& m1 = *this;
	const FMatrix3& m2 = InMat;

	//From GLM
	using T = float;
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

	FMatrix3 Result;
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

FMatrix3& FMatrix3::operator*=(const FMatrix3& InMat)
{
	return (*this = *this * InMat);
}

FMatrix3 FMatrix3::FromEuler(const FRotator& InRot)
{
	float rPitch = FMaths::Radians(InRot.Pitch);
	float rYaw = FMaths::Radians(InRot.Yaw);
	float rRoll = FMaths::Radians(InRot.Roll);

	//From GLM
	using T = float;
	FMatrix3 Result(T(1.0F));
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

FMatrix3 FMatrix3::FromQuat(const FQuaternion& InQuat)
{
	//From GLM
	using T = float;
	FMatrix3 Result(T(1.0F));
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

FMatrix3 FMatrix3::FromScale(const FVector3& InScale)
{
	FMatrix3 Result;
	Result[0][0] = InScale.X;
	Result[1][1] = InScale.Y;
	Result[2][2] = InScale.Z;
	return Result;
}


}