#pragma once
#include "OpenGLShader.h"
#include "Rev/Core/FileSystem.h"

namespace Rev
{

struct FOpenGLCompiledShaderData
{
	std::string Name;
	std::map<ERHIShaderStage, std::vector<uint32_t>> CompiledDataMap;
};

class FOpenGLShaderFactory
{
public:
	static const char* GetCacheDirectory();
	static const char* GetCachedFileExtension(ERHIShaderStage InStage);
	static void CreateCacheDirectory();

	static void DumpShaderInfo(const std::string& InName, ERHIShaderStage InStage, const std::vector<uint32_t>& InData);
	static FOpenGLCompiledShaderData LoadAndCompile(const FPath& InPath, const std::string& InName);
	static FRHIGraphicsShaders CreateGraphicsShaders(const FOpenGLCompiledShaderData& InData);
	//CreateComputeShader
	static Ref<FOpenGLShaderProgram> CreateShaderProgram(const std::string& InName, const FRHIGraphicsShaders& InShaders);


};

}