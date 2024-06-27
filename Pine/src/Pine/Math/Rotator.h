#pragma once
#include "Pine/Core/Base.h"
#include "Pine/Math/Vector.h"
#include "Pine/Math/Quaternion.h"
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Pine
{

struct FRotator
{
public:
	float pitch; //InDegree
	float yaw; //InDegree
	float roll; //InDegree

	FRotator()
		: pitch(0.0f)
		, yaw(0.0f)
		, roll(0.0f)
	{}

	FRotator(float a)
		: pitch(a)
		, yaw(a)
		, roll(a)
	{}

	FRotator(float _pitch, float _yaw, float _roll)
		: pitch(_pitch)
		, yaw(_yaw)
		, roll(_roll)
	{}

	FRotator(const FRotator& r)
		: pitch(r.pitch)
		, yaw(r.yaw)
		, roll(r.roll)
	{}

	FRotator(FRotator&& r) noexcept
		: pitch(std::move(r.pitch))
		, yaw(std::move(r.yaw))
		, roll(std::move(r.roll))
	{}

	inline FRotator& operator=(const FRotator& r)
	{
		pitch = r.pitch;
		yaw = r.yaw;
		roll = r.roll;
		return *this;
	}

	inline FRotator& operator=(FRotator&& r) noexcept
	{
		pitch = std::move(r.pitch);
		yaw = std::move(r.yaw);
		roll = std::move(r.roll);
		return *this;
	}

	inline FRotator operator+(const FRotator& r) const
	{
		return FRotator(pitch + r.pitch, yaw + r.yaw, roll + r.roll);
	}

	inline FRotator& operator+=(const FRotator& r)
	{
		pitch += r.pitch;
		yaw += r.yaw;
		roll += r.roll;
		return *this;
	}

	inline FRotator operator-(const FRotator& r) const
	{
		return FRotator(pitch - r.pitch, yaw - r.yaw, roll - r.roll);
	}

	inline FRotator& operator-=(const FRotator& r)
	{
		pitch -= r.pitch;
		yaw -= r.yaw;
		roll -= r.roll;
		return *this;
	}

	inline bool operator==(const FRotator& r) const
	{
		return pitch==r.pitch && yaw==r.yaw && roll==r.roll;
	}

	inline bool operator!=(const FRotator& r) const
	{
		return pitch != r.pitch || yaw != r.yaw || roll != r.roll;
	}
};

}