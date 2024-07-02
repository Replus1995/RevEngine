#pragma once
#include "Matrix3.h"
#include "Vector4.h"

namespace Rev::Math
{

struct FMatrix4
{
public:
	FVector4 Columns[4];

	FMatrix4();
	FMatrix4(float InScalar);
	FMatrix4(FVector4 InCol0, FVector4 InCol1, FVector4 InCol2, FVector4 InCol3);
	FMatrix4(FMatrix3 InMat);

	FVector4& operator[](int Index);
	FVector4 const& operator[](int Index) const;
	float const* DataPtr() const;

	FMatrix4 operator*(const FMatrix4& InMat) const;
	FMatrix4& operator*=(const FMatrix4& InMat);

	FMatrix4 Inverse() const;

	static FMatrix4 FromTranslate(const FVector3& InTrans);
	static FMatrix4 FromEuler(const FRotator& InRot);
	static FMatrix4 FromQuat(const FQuaternion& InQuat);
	static FMatrix4 FromScale(const FVector3& InScale);

	static FMatrix4 FromTRS(const FVector3& InT, const FRotator& InR, const FVector3& InS);
	static FMatrix4 FromTRS(const FVector3& InT, const FQuaternion& InQ, const FVector3& InS);

	static FMatrix4 Perspective(float InFOV, float InAspectRatio, float InNear, float InFar);
	static FMatrix4 Othographic(float InLeft, float InRight, float InBottom, float InTop, float InNear, float InFar);
};

}

#include "Matrix4.inl"


