#pragma once
#include "Platform/Shaderc/ShadercUtils.h"

namespace Rev
{

class FShadercFactory
{
public:
	static void CompileShaders(const FShadercSource& InSource, FShadercCompiledData& OutData);
	static FShadercCompiledData LoadAndCompile(const FPath& InPath);
};

}