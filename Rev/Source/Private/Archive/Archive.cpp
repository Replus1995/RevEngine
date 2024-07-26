#include "Rev/Archive/Archive.h"

namespace Rev
{

FArchive& operator<<(FArchive& Ar, std::string& Value)
{
    uint64 StrLength = 0;
    if (Ar.IsLoading())
    {
        Ar << StrLength;
        Value.resize(StrLength, 0);
        Ar.Serialize(Value.data(), StrLength);
    }
    else
    {
        StrLength = Value.length();
        Ar << StrLength;
        Ar.Serialize(Value.data(), StrLength);
    }
    return Ar;
}

}


