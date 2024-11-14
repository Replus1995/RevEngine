#include "Rev/Core/Hash.h"
#include <city.h>


namespace Rev
{

uint32 FCityHash::Gen(const void* InData, size_t InLength)
{
	return CityHash32((const char*)InData, InLength);
}

uint32 FCityHash::Gen(const char* InStr)
{
	return Gen(std::string_view(InStr));
}

uint32 FCityHash::Gen(std::string_view InStr)
{
	return Gen(InStr.data(), InStr.length());
}

FHashEntry::FHashEntry(uint32 InValue)
	: mValue(InValue)
{
}

FHashEntry::~FHashEntry()
{
}

}


