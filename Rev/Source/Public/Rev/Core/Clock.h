#pragma once
#include <chrono>

namespace Rev
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
		return ElapsedMillis() * 0.001f;
	}

	float Clock::ElapsedMillis()
	{
		return ElapsedMicros() * 0.001f;
	}

	int64 Clock::ElapsedMicros()
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - mStart).count();
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> mStart;
};

}