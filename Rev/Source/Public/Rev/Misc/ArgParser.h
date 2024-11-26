#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{

class REV_API FArgParser
{
public:
	FArgParser() = default;
	FArgParser(const FArgParser&) = delete;
	~FArgParser() = default;

	void ParseArgs(uint32 InNumArgs, char** InArgs);
	const char* GetRawArg(uint32 ArgIndex);

private:
	uint32 NumRawArgs = 0;
	char** RawArgs = nullptr;
};
REV_API extern FArgParser GArgParser;

}