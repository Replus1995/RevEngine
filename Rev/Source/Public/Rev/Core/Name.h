#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{

class FName
{
public:
	FName();
	FName(const FName& Other);
	FName(const std::string& InStr);

	std::string ToString();

	FName& operator=(const FName& Other);

	bool operator<(const FName& Other) const;
	bool operator>(const FName& Other) const;
	bool operator==(const FName& Other) const;
	bool operator!=(const FName& Other) const;

	bool IsNone() const;

private:
	uint32 mCompareIndex;
	uint32 mNumber;
};

}