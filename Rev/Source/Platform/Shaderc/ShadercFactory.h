#pragma once
#include "Shaderc/ShadercUtils.h"

namespace Rev
{

class FShadercFactory
{
public:
	static void ReflectUniformInfo(FShadercCompiledData& Data);
	static void CompileShaders(const FShadercSource& InSource, const FRHIShaderCompileOptions& InOptions, FShadercCompiledData& OutData);
	static FShadercCompiledData LoadOrCompileShader(const FPath& InPath, const FRHIShaderCompileOptions& InOptions = {}, ERHIShaderStage InStage = ERHIShaderStage::Unknown);

public:
	static ERHIShaderStage DetectShaderStage(const FShadercSource& InSource);
};

}