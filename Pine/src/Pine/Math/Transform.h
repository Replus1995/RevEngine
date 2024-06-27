#pragma once
#include "Pine/Core/Base.h"
#include "Pine/Math/Math.h"

namespace Pine
{

struct PINE_API FTransform
{
public:
	FVector3 Location = FVector3(1.0f);
	FRotator Rotation = FRotator(0.0f);
	FVector3 Scale = FVector3(1.0f);

	inline FMatrix4 ToMatrix() const
	{
		return FMaths::MakeMatrix(Location, Rotation, Scale);
	}

	inline FVector3 Right() const
	{
		return FMaths::Right(Rotation);
	}

	inline FVector3 Up() const
	{
		return FMaths::Up(Rotation);
	}
	
	inline FVector3 Forward() const
	{
		return FMaths::Forward(Rotation);
	}
};

}