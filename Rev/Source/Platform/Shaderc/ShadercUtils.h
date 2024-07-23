#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Buffer.h"
#include "Rev/Core/FileSystem.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RHI/RHIShader.h"
#include <map>
#include <filesystem>

namespace Rev
{

struct FShadercSource
{
	FPath FilePath;
	FBuffer FileContent;
	std::map<ERHIShaderStage, FBufferView> StageSourceMap;
};

struct FShadercCompiledData
{
	std::string Name;
	std::map<ERHIShaderStage, std::vector<uint32_t>> CompiledDataMap;

	bool Empty() const
	{
		return CompiledDataMap.empty();
	}
};

class FShadercUtils
{
public:
	static const char* GetCacheDirectory();
	static void CreateCacheDirectory();
	static const char* GetCacheExtension();

	static ERHIShaderStage StringToShaderStage(std::string_view InStr);
	static const char* ShaderStageToString(ERHIShaderStage InStage);

	static FShadercSource LoadShaderSource(const FPath& InPath);
	static bool LoadShaderCompiledData(const std::filesystem::path& ShaderCachePath, FShadercCompiledData& OutCompiledData);
	static bool SaveShaderCompiledData(const std::filesystem::path& ShaderCachePath, const FShadercCompiledData& InCompiledData);
	static void DumpShaderInfo(const FShadercCompiledData& InData);
};

}