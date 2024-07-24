#pragma once
#include "Platform/Shaderc/ShadercUtils.h"

namespace Rev
{

class FShadercFactory
{
public:
	static void CompileShaders(const FShadercSource& InSource, const FRHIShaderCompileOptions& InOptions, FShadercCompiledData& OutData);
	static FShadercCompiledData LoadOrCompileShader(const FPath& InPath, const FRHIShaderCompileOptions& InOptions = {});
};

}