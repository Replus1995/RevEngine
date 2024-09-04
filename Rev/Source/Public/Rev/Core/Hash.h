#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{

class FCityHash
{
public:
	FCityHash(uint64 InSeed = 0);

	uint64 Build(const void* InData, size_t InLength);
	uint64 Build(const char* InStr);
	uint64 Build(std::string_view InStr);

	template<typename T>
	uint64 Build(const T& InType)
	{
		return Build(&InType, sizeof(T));
	}

private:
	uint64 mSeed;
};

}