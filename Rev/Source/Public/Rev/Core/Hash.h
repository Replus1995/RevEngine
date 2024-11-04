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
	static uint32 Gen(const T& InType)
	{
		return Gen(&InType, sizeof(T));
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

#define STD_MAP_HASH(KeyClass)						\
template<>											\
struct std::hash<KeyClass>							\
{													\
	std::size_t operator()(const KeyClass& k) const \
	{												\
		return Rev::FCityHash::Gen<KeyClass>(k);	\
	}												\
};