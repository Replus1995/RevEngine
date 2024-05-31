#pragma once
#include <chrono>

namespace Pine
{

class Clock
{
public:
	Clock()
	{
		Reset();
	}

	void Clock::Reset()
	{
		mStart = std::chrono::high_resolution_clock::now();
	}

	float Clock::Elapsed()
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - mStart).count() * 0.001f * 0.001f * 0.001f;
	}

	float Clock::ElapsedMillis()
	{
		return Elapsed() * 1000.0f;
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> mStart;
};

}