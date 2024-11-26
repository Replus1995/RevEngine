#include "Rev/Misc/ArgParser.h"

namespace Rev
{
FArgParser GArgParser;

void Rev::FArgParser::ParseArgs(uint32 InNumArgs, char** InArgs)
{
	NumRawArgs = InNumArgs;
	RawArgs = InArgs;
}

const char* FArgParser::GetRawArg(uint32 ArgIndex)
{
	return ArgIndex < NumRawArgs ? RawArgs[ArgIndex] : nullptr;
}

}


