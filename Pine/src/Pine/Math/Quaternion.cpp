#include "pinepch.h"
#include "Quaternion.h"
#include "Maths.h"
#include <cmath>

namespace Pine
{

template<typename T>
TQuaternion<T>::TQuaternion()
	: X(0.0F)
	, Y(0.0F)
	, Z(0.0F)
	, W(1.0F)
{
}

template<typename T>
TQuaternion<T>::TQuaternion(T InX, T InY, T InZ, T InW)
	: X(InX)
	, Y(InY)
	, Z(InZ)
	, W(InW)
{
}

template<typename T>
TQuaternion<T>::TQuaternion(const TQuaternion<T>& InQuat)
	: X(InQuat.X)
	, Y(InQuat.Y)
	, Z(InQuat.Z)
	, W(InQuat.W)
{
}

template<typename T>
bool TQuaternion<T>::operator==(const TQuaternion<T>& InQuat) const
{
	return X == InQuat.X && Y == InQuat.Y && Z == InQuat.Z && W == InQuat.W;
}

template<typename T>
bool TQuaternion<T>::operator!=(const TQuaternion<T>& InQuat) const
{
	return X != InQuat.X || Y != InQuat.Y || Z != InQuat.Z || W != InQuat.W;
}

template<typename T>
TQuaternion<T> TQuaternion<T>::operator-() const
{
	return TQuaternion<T>(-X, -Y, -Z, -W);
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::operator=(const TQuaternion<T>& InQuat)
{
	X = InQuat.X;
	Y = InQuat.Y;
	Z = InQuat.Z;
	W = InQuat.W;
	return *this;
}

template<typename T>
TQuaternion<T> TQuaternion<T>::operator+(const TQuaternion<T>& InQuat) const
{
	return TQuaternion<T>(X + InQuat.X, Y + InQuat.Y, Z + InQuat.Z, W + InQuat.W);
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::operator+=(const TQuaternion<T>& InQuat)
{
	X += InQuat.X;
	Y += InQuat.Y;
	Z += InQuat.Z;
	W += InQuat.W;
	return *this;
}

template<typename T>
TQuaternion<T> TQuaternion<T>::operator-(const TQuaternion<T>& InQuat) const
{
	return TQuaternion<T>(X - InQuat.X, Y - InQuat.Y, Z - InQuat.Z, W - InQuat.W);;
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::operator-=(const TQuaternion<T>& InQuat)
{
	X -= InQuat.X;
	Y -= InQuat.Y;
	Z -= InQuat.Z;
	W -= InQuat.W;
	return *this;
}

template<typename T>
TQuaternion<T> TQuaternion<T>::operator*(const TQuaternion<T>& InQuat) const
{
	TQuaternion<T> Result(*this);
	Result *= InQuat;
	return Result;
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::operator*=(const TQuaternion<T>& InQuat)
{
	//From GLM
	const TQuaternion<T> p(*this);
	const TQuaternion<T>& q = InQuat;

	W = p.W * q.W - p.X * q.X - p.Y * q.Y - p.Z * q.Z;
	X = p.W * q.X + p.X * q.W + p.Y * q.Z - p.Z * q.Y;
	Y = p.W * q.Y + p.Y * q.W + p.Z * q.X - p.X * q.Z;
	Z = p.W * q.Z + p.Z * q.W + p.X * q.Y - p.Y * q.X;
	return *this;
}

template<typename T>
TQuaternion<T> TQuaternion<T>::operator*(T InScalar) const
{
	return TQuaternion<T>(X * InScalar, Y * InScalar, Z * InScalar, W * InScalar);
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::operator*=(T InScalar)
{
	X *= InScalar;
	Y *= InScalar;
	Z *= InScalar;
	W *= InScalar;
	return *this;
}

template<typename T>
TQuaternion<T> TQuaternion<T>::operator/(T InScalar) const
{
	return TQuaternion<T>(X / InScalar, Y / InScalar, Z / InScalar, W / InScalar);
}

template<typename T>
TQuaternion<T>& TQuaternion<T>::operator/=(T InScalar)
{
	X /= InScalar;
	Y /= InScalar;
	Z /= InScalar;
	W /= InScalar;
	return *this;
}

template<typename T>
TVector3<T> TQuaternion<T>::operator*(const TVector3<T>& InVec) const
{
	TQuaternion<T> Result = (*this) * TQuaternion<T>(InVec.X, InVec.Y, InVec.Z, 0.0F) * Conjugate();
	return TVector3<T>(Result.X, Result.Y, Result.Z);
}

template<typename T>
TQuaternion<T> TQuaternion<T>::Conjugate() const
{
	return TQuaternion<T>(-X, -Y, -Z, W);
}

template<typename T>
void TQuaternion<T>::Normalize()
{
	T Mag = std::sqrt(X * X + Y * Y + Z * Z + W * W);
	if (Mag > 0.0F) {
		T InvMag = 1.0F / Mag;
		X *= InvMag;
		Y *= InvMag;
		Z *= InvMag;
		W *= InvMag;
	}
}

template<typename T>
TVector3<T> TQuaternion<T>::Right() const
{
	return (*this) * TVector3<T>(1.0F, 0.0F, 0.0F);
}

template<typename T>
TVector3<T> TQuaternion<T>::Up() const
{
	return (*this) * TVector3<T>(0.0F, 1.0F, 0.0F);
}

template<typename T>
TVector3<T> TQuaternion<T>::Forward() const
{
	return (*this) * TVector3<T>(0.0F, 0.0F, -1.0F);
}

template<typename T>
TQuaternion<T> TQuaternion<T>::FromAxisAngle(const TVector3<T>& InAxis, float InDegree)
{
	T ra = FMaths::Radians(InDegree);
	T sa = std::sin(ra / 2.0F);
	T ca = std::cos(ra / 2.0F);

	return TQuaternion<T>(InAxis.X * sa, InAxis.Y * sa, InAxis.Z * sa, ca);
}

template<typename T>
std::pair<const TVector3<T>, float> TQuaternion<T>::ToAxisAngle(const TQuaternion<T>& InQuat)
{
	float degree = FMaths::Degrees(std::acos(InQuat.W) * 2.0F);
	T sa = std::sqrt(1.0F - InQuat.W * InQuat.W);
	TVector3<T> axis(InQuat.X / sa, InQuat.Y / sa, InQuat.Z / sa);
	return std::pair<const TVector3<T>, float>(axis, degree);
}

template<typename T>
TQuaternion<T> TQuaternion<T>::FromEuler(const TRotator<T>& InRot)
{
	T rPitch = FMaths::Radians(InRot.Pitch);
	T rYaw = FMaths::Radians(InRot.Yaw);
	T rRoll = FMaths::Radians(InRot.Roll);

	T cp = std::cos(rPitch / 2);
	T sp = std::sin(rPitch / 2);
	T cy = std::cos(rYaw / 2);
	T sy = std::sin(rYaw / 2);
	T cr = std::cos(rRoll / 2);
	T sr = std::sin(rRoll / 2);

	TQuaternion Quat;
	Quat.W = cp * cy * cr + sp * sy * sr;
	Quat.X = sp * cy * cr + cp * sy * sr;
	Quat.Y = cp * sy * cr - sp * cy * sr;
	Quat.Z = cp * cy * sr - sp * sy * cr;

	return Quat;
}

template<typename T>
TRotator<T> TQuaternion<T>::ToEuler(const TQuaternion<T>& InQuat)
{
	T r11 = 2 * (InQuat.X * InQuat.Z + InQuat.W * InQuat.Y);
	T r12 = InQuat.W * InQuat.W - InQuat.X * InQuat.X - InQuat.Y * InQuat.Y + InQuat.Z * InQuat.Z;
	T r21 = -2 * (InQuat.Y * InQuat.Z - InQuat.W * InQuat.X);
	T r31 = 2 * (InQuat.X * InQuat.Y + InQuat.W * InQuat.Z);
	T r32 = InQuat.W * InQuat.W - InQuat.X * InQuat.X + InQuat.Y * InQuat.Y - InQuat.Z * InQuat.Z;

	TRotator<T> Result;
	Result.Yaw = FMaths::Degrees(std::atan2(r11, r12));
	Result.Pitch = FMaths::Degrees(std::asin(r21));
	Result.Roll = FMaths::Degrees(std::atan2(r31, r32));

	return Result;
}

template<typename T>
T TQuaternion<T>::DotProduct(const TQuaternion<T>& InQuatA, const TQuaternion<T>& InQuatB)
{
	return (InQuatA.X * InQuatB.X) + (InQuatA.Y * InQuatB.Y) + (InQuatA.Z * InQuatB.Z) + (InQuatA.W * InQuatB.W);
}

template<typename T>
TQuaternion<T> TQuaternion<T>::Lerp(const TQuaternion<T>& InQuatA, const TQuaternion<T>& InQuatB, float By)
{
	By = FMaths::Clamp(By, 0.0F, 1.0F);
	float dot = DotProduct(InQuatA, InQuatB);
	if (dot < 0.0f)
	{
		return (InQuatA * (1.0F - By)) + (-InQuatB * By);
	}
	else
	{
		return (InQuatA * (1.0F - By)) + (InQuatB * By);
	}
}

template<typename T>
TQuaternion<T> TQuaternion<T>::Slerp(const TQuaternion<T>& InQuatA, const TQuaternion<T>& InQuatB, float By)
{
	By = FMaths::Clamp(By, 0.0F, 1.0F);
	float cBy = std::cos(By * FMaths::PI());
	float dot = DotProduct(InQuatA, InQuatB);
	if (dot < 0.0f)
	{
		return (InQuatA * cBy) + (-InQuatB * (1.0F - cBy));
	}
	else
	{
		return (InQuatA * cBy) + (InQuatB * (1.0F - cBy));
	}
}

}


