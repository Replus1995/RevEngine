#pragma once
#include "Shaderc/ShadercUtils.h"

namespace Rev
{

class FShadercFactory
{
public:
	static void ReflectShaderInfo(FShadercCompiledData& Data);
	static void CompileShaders(const FShadercSource& InSource, const FRHIShaderCompileOptions& InOptions, FShadercCompiledData& OutData);
	static FShadercCompiledData LoadOrCompileShader(const char* InPath, const FRHIShaderCompileOptions& InOptions = {}, EShaderStage InStage = SS_Unknown);

public:
	static EShaderStage DetectShaderStage(const FShadercSource& InSource);
};

}