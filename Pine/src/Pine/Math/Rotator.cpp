#include "pinepch.h"
#include "Rotator.h"
#include "Maths.h"
#include <cmath>

namespace Pine
{

template<typename T>
TRotator<T>::TRotator() noexcept
	: Pitch(0.0F)
	, Yaw(0.0F)
	, Roll(0.0F)
{
}

template<typename T>
TRotator<T>::TRotator(T InScale) noexcept
	: Pitch(InScale)
	, Yaw(InScale)
	, Roll(InScale)
{
}

template<typename T>
TRotator<T>::TRotator(T InPitch, T InYaw, T InRoll) noexcept
	: Pitch(InPitch)
	, Yaw(InYaw)
	, Roll(InRoll)
{
}

template<typename T>
TRotator<T>::TRotator(const TRotator<T>& InRot) noexcept
	: Pitch(InRot.Pitch)
	, Yaw(InRot.Yaw)
	, Roll(InRot.Roll)
{
}

template<typename T>
bool TRotator<T>::operator==(const TRotator<T>& InRot) const
{
	return Pitch == InRot.Pitch && Yaw == InRot.Yaw && Roll == InRot.Roll;
}

template<typename T>
bool TRotator<T>::operator!=(const TRotator<T>& InRot) const
{
	return Pitch != InRot.Pitch || Yaw != InRot.Yaw || Roll != InRot.Roll;
}

template<typename T>
TRotator<T>& TRotator<T>::operator=(const TRotator<T>& InRot) noexcept
{
	Pitch = InRot.Pitch;
	Yaw = InRot.Yaw;
	Roll = InRot.Roll;
	return *this;
}

template<typename T>
TRotator<T> TRotator<T>::operator+(const TRotator<T>& InRot) const
{
	return TRotator<T>(Pitch + InRot.Pitch, Yaw + InRot.Yaw, Roll + InRot.Roll);
}

template<typename T>
TRotator<T>& TRotator<T>::operator+=(const TRotator<T>& InRot)
{
	Pitch += InRot.Pitch;
	Yaw += InRot.Yaw;
	Roll += InRot.Roll;
	return *this;
}

template<typename T>
TRotator<T> TRotator<T>::operator-(const TRotator<T>& InRot) const

{
	return TRotator<T>(Pitch - InRot.Pitch, Yaw - InRot.Yaw, Roll - InRot.Roll);
}

template<typename T>
TRotator<T>& TRotator<T>::operator-=(const TRotator<T>& InRot)
{
	Pitch -= InRot.Pitch;
	Yaw -= InRot.Yaw;
	Roll -= InRot.Roll;
	return *this;
}

template<typename T>
TVector3<T> TRotator<T>::Right() const
{
	T rPitch = FMaths::Radians(Pitch);
	T rYaw = FMaths::Radians(Yaw);
	T rRoll = FMaths::Radians(Roll);

	T tmp_ch = std::cos(rYaw);
	T tmp_sh = std::sin(rYaw);
	T tmp_cp = std::cos(rPitch);
	T tmp_sp = std::sin(rPitch);
	T tmp_cb = std::cos(rRoll);
	T tmp_sb = std::sin(rRoll);

	TVector3<T> Result;
	Result[0] = tmp_ch * tmp_cb + tmp_sh * tmp_sp * tmp_sb;
	Result[1] = tmp_sb * tmp_cp;
	Result[2] = -tmp_sh * tmp_cb + tmp_ch * tmp_sp * tmp_sb;

	return Result;
}

template<typename T>
TVector3<T> TRotator<T>::Up() const
{
	T rPitch = FMaths::Radians(Pitch);
	T rYaw = FMaths::Radians(Yaw);
	T rRoll = FMaths::Radians(Roll);

	T tmp_ch = std::cos(rYaw);
	T tmp_sh = std::sin(rYaw);
	T tmp_cp = std::cos(rPitch);
	T tmp_sp = std::sin(rPitch);
	T tmp_cb = std::cos(rRoll);
	T tmp_sb = std::sin(rRoll);

	TVector3<T> Result;
	Result[0] = -tmp_ch * tmp_sb + tmp_sh * tmp_sp * tmp_cb;
	Result[1] = tmp_cb * tmp_cp;
	Result[2] = tmp_sb * tmp_sh + tmp_ch * tmp_sp * tmp_cb;

	return Result;
}

template<typename T>
TVector3<T> TRotator<T>::Forward() const
{
	T rPitch = FMaths::Radians(Pitch);
	T rYaw = FMaths::Radians(Yaw);

	T tmp_ch = std::cos(rYaw);
	T tmp_sh = std::sin(rYaw);
	T tmp_cp = std::cos(rPitch);
	T tmp_sp = std::sin(rPitch);

	TVector3<T> Result;
	Result[0] = tmp_sh * tmp_cp;
	Result[1] = -tmp_sp;
	Result[2] = tmp_ch * tmp_cp;
	Result *= T(-1.0F);
	return Result;
}

}