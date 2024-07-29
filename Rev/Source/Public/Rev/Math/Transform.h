#pragma once
#include "Vector3.h"
#include "Rotator.h"
#include "Matrix4.h"

namespace Rev
{
namespace Math
{

struct FTransform
{
public:
	FVector3 Location = FVector3(0.0F);
	FRotator Rotation = FRotator(0.0F);
	FVector3 Scale = FVector3(1.0F);

	inline FMatrix4 ToMatrix() const
	{
		return FMatrix4::FromTRS(Location, Rotation, Scale);
	}

	inline FVector3 Right() const
	{
		return Rotation.Right();
	}

	inline FVector3 Up() const
	{
		return Rotation.Up();
	}

	inline FVector3 Forward() const
	{
		return Rotation.Forward();
	}
};

}
}