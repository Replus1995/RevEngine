#pragma once
#include "Vector3.h"
#include "Rotator.h"
#include "Quaternion.h"

namespace Pine
{

struct PINE_API FMatrix3
{
public:
	FVector3 Columns[3];

	FMatrix3();
	FMatrix3(float InScalar);

	FVector3& operator[](int Index);
	FVector3 const& operator[](int Index) const;
	float const* DataPtr() const;

	FMatrix3 operator*(const FMatrix3& InMat) const;
	FMatrix3& operator*=(const FMatrix3& InMat);

	static FMatrix3 FromEuler(const FRotator& InRot);
	static FMatrix3 FromQuat(const FQuaternion& InQuat);
	static FMatrix3 FromScale(const FVector3& InScale);
};

}