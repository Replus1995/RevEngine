#pragma once
#include "Rotator.h"
#include "Vector3.h"
#include <utility>

namespace Rev
{
namespace Math
{

template<typename T>
struct TQuaternion
{
	static_assert(std::is_floating_point_v<T>, "T must be floating point");
public:
	T X;
	T Y;
	T Z;
	T W;

	TQuaternion();
	TQuaternion(T InX, T InY, T InZ, T InW);
	TQuaternion(const TQuaternion<T>& InQuat);

	bool operator==(const TQuaternion<T>& InQuat) const;
	bool operator!=(const TQuaternion<T>& InQuat) const;
	TQuaternion<T> operator-() const;
	TQuaternion<T>& operator=(const TQuaternion<T>& InQuat);

	TQuaternion<T> operator+(const TQuaternion<T>& InQuat) const;
	TQuaternion<T>& operator+=(const TQuaternion<T>& InQuat);
	TQuaternion<T> operator-(const TQuaternion<T>& InQuat) const;
	TQuaternion<T>& operator-=(const TQuaternion<T>& InQuat);
	TQuaternion<T> operator*(const TQuaternion<T>& InQuat) const;
	TQuaternion<T>& operator*=(const TQuaternion<T>& InQuat);
	TQuaternion<T> operator*(T InScalar) const;
	TQuaternion<T>& operator*=(T InScalar);
	TQuaternion<T> operator/(T InScalar) const;
	TQuaternion<T>& operator/=(T InScalar);

	TVector3<T> operator*(const TVector3<T>& InVec) const;

	TQuaternion<T> Conjugate() const;
	void Normalize();

	TVector3<T> Right() const;
	TVector3<T> Up() const;
	TVector3<T> Forward() const;

	static TQuaternion<T> WXYZ(T InW, T InX, T InY, T InZ);

	static TQuaternion<T> FromAxisAngle(const TVector3<T>& InAxis, float InDegree);
	static std::pair<const TVector3<T>, float> ToAxisAngle(const TQuaternion<T>& InQuat);

	static TQuaternion<T> FromEuler(const TRotator<T>& InRot);
	static TRotator<T> ToEuler(const TQuaternion<T>& InQuat);

	static T DotProduct(const TQuaternion<T>& InQuatA, const TQuaternion<T>& InQuatB);
	static TQuaternion<T> Lerp(const TQuaternion<T>& InQuatA, const TQuaternion<T>& InQuatB, float By);
	static TQuaternion<T> Slerp(const TQuaternion<T>& InQuatA, const TQuaternion<T>& InQuatB, float By);
};

using FQuaternion = TQuaternion<float>;

}
}

#include "Quaternion.inl"


