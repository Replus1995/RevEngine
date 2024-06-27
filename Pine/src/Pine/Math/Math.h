#pragma once
#include "Pine/Core/Base.h"
#include "Pine/Math/Vector.h"
#include "Pine/Math/Rotator.h"
#include "Pine/Math/Quaternion.h"
#include "Pine/Math/Matrix.h"


namespace Pine
{

class PINE_API FMaths
{
public:
	static float Radians(float degree);
	static float Degrees(float radian);

	static FRotator MakeRotator(const FQuaternion& q);
	static FQuaternion MakeQuaternion(const FRotator& r);

	static FMatrix4 MakeMatrix(const FRotator& r);
	static FMatrix4 MakeMatrix(const FQuaternion& q);
	static FMatrix4 MakeMatrix(const FVector3& t, const FRotator& r, const FVector3& s);
	static FMatrix4 MakeMatrix(const FVector3& t, const FQuaternion& q, const FVector3& s);

	static bool Decompose(const FMatrix4& m, FVector3& t, FRotator& r, FVector3& s);
	//static bool Decompose(const Matrix4& m, Vector3& t, Quaternion& q, Vector3& s);

	static FVector3 Right(const FRotator& r);
	static FVector3 Up(const FRotator& r);
	static FVector3 Forward(const FRotator& r);

	static float Clamp(float val, float minVal, float maxVal);
};

}