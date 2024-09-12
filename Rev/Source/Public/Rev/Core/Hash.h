#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{

class FCityHash
{
public:
	static uint32 Gen(const void* InData, size_t InLength);
	static uint32 Gen(const char* InStr);
	static uint32 Gen(std::string_view InStr);

	template<typename T>
	static uint64 Gen(const T& InType)
	{
		return Build(&InType, sizeof(T));
	}
};

class FHashEntry
{
public:
	FHashEntry(uint32 InValue);
	virtual ~FHashEntry();

private:
	uint32 mValue = 0;
};

}