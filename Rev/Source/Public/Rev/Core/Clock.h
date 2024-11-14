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

	void Reset()
	{
		mStart = std::chrono::high_resolution_clock::now();
	}

	float Elapsed()
	{
		return ElapsedMillis() * 0.001f;
	}

	float ElapsedMillis()
	{
		return ElapsedMicros() * 0.001f;
	}

	int64 ElapsedMicros()
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - mStart).count();
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> mStart;
};

}