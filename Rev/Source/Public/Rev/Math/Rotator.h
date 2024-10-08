#pragma once
#include "Vector3.h"

namespace Rev
{
namespace Math
{

//Rotate order is Roll->Pitch->Yaw(glm::YXZ)
template<typename T>
struct TRotator
{
	static_assert(std::is_floating_point_v<T>, "T must be floating point");
public:
	T Pitch;
	T Yaw;
	T Roll;

	TRotator() noexcept;
	TRotator(T InScale) noexcept;
	TRotator(T InPitch, T InYaw, T InRoll) noexcept;
	TRotator(const TRotator<T>& InRot) noexcept;

	bool operator==(const TRotator<T>& InRot) const;
	bool operator!=(const TRotator<T>& InRot) const;
	TRotator<T>& operator=(const TRotator<T>& InRot) noexcept;

	TRotator<T> operator+(const TRotator<T>& InRot) const;
	TRotator<T>& operator+=(const TRotator<T>& InRot);
	TRotator<T> operator-(const TRotator<T>& InRot) const;
	TRotator<T>& operator-=(const TRotator<T>& InRot);

	TVector3<T> Right() const;
	TVector3<T> Up() const;
	TVector3<T> Forward() const;
};

using FRotator = TRotator<float>;

}
}

#include "Rotator.inl"

