#include "Rev/Core/Hash.h"
#include <city.h>


namespace Rev
{

FCityHash::FCityHash(uint64 InSeed)
	: mSeed(InSeed)
{
}

uint64 FCityHash::Build(const void* InData, size_t InLength)
{
	return CityHash64WithSeed((const char*)InData, InLength, mSeed);
}

uint64 FCityHash::Build(const char* InStr)
{
	return Build(std::string_view(InStr));
}

uint64 FCityHash::Build(std::string_view InStr)
{
	return Build(InStr.data(), InStr.length());
}

}


