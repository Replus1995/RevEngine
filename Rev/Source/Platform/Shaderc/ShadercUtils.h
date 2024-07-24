#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Buffer.h"
#include "Rev/Core/FileSystem.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RHI/RHIShader.h"
#include "Rev/Render/RHI/RHIShaderCompile.h"
#include <map>
#include <filesystem>

namespace Rev
{

struct FShadercSource
{
	FPath FilePath;
	FBuffer FileContent;
	ERHIShaderStage Stage = ERHIShaderStage::Unknown;
};

struct FShadercCompiledData
{
	std::string Name;
	FBuffer CompiledData;
	ERHIShaderStage Stage = ERHIShaderStage::Unknown;

	bool Empty() const
	{
		return CompiledData.Empty();
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