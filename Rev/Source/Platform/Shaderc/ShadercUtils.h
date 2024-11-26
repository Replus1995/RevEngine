#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Buffer.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/PixelFormat.h"
#include "Rev/Render/RHI/RHIShader.h"
#include "Rev/Render/RHI/RHIShaderCompile.h"
#include "Rev/Render/RHI/RHITexture.h"
#include <map>
#include <filesystem>

namespace Rev
{

struct FShadercSource
{
	std::string FilePath;
	FBuffer FileContent;
	EShaderStage Stage = SS_Unknown;

	FShadercSource() = default;
	FShadercSource(FShadercSource&& Other) noexcept
		: FilePath(std::move(Other.FilePath))
		, FileContent(std::move(Other.FileContent))
		, Stage(Other.Stage)
	{
	}
};

struct FShadercCompiledData
{
	std::string Name;
	EShaderStage Stage = SS_Unknown;
	FBuffer Binary;
	std::vector<FRHIShaderUniform> Uniforms;
	std::vector<FRHIShaderAttribute> Attributes; //only for vertex shader input

	bool Empty() const
	{
		return Binary.Empty();
	}

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, FShadercCompiledData& Data)
	{
		//Ar << Data.Name;
		Ar << Data.Stage;
		Ar << Data.Binary;
		return Ar;
	}
};

class FShadercUtils
{
public:
	static const char* GetCacheDirectory();
	static void CreateCacheDirectory();
	static const char* GetCacheExtension();

	static EShaderStage StringToShaderStage(std::string_view InStr);
	static const char* ShaderStageToString(EShaderStage InStage);

	static FShadercSource LoadShaderSource(const char* InPath);
	static bool LoadShaderCompiledData(const std::filesystem::path& ShaderCachePath, FShadercCompiledData& OutCompiledData);
	static bool SaveShaderCompiledData(const std::filesystem::path& ShaderCachePath, FShadercCompiledData& InCompiledData);
};

}