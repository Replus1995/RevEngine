#include "ShadercUtils.h"
#include "Rev/Core/Log.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Clock.h"
#include "Rev/Archive/FileArchive.h"

#include <vector>
#include <filesystem>
#include <regex>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_reflect.hpp>
#include <spirv-tools/optimizer.hpp>

#include <iostream>

namespace fs = std::filesystem;

namespace Rev
{

namespace
{

template<typename T>
bool ReadData(std::ifstream& ifile, T& value)
{
	uint32 ElemSize = 0;
	uint32 Count = 0;
	ifile.read((char*)&ElemSize, sizeof(uint32));
	ifile.read((char*)&Count, sizeof(uint32));
	if(ElemSize != sizeof(T) || Count != 1)
		return false;
	ifile.read((char*)&value, ElemSize * Count);
}

template<typename T>
bool ReadData(std::ifstream& ifile, std::vector<T>& values)
{
	uint32 ElemSize = 0;
	uint32 Count = 0;
	ifile.read((char*)&ElemSize, sizeof(uint32));
	ifile.read((char*)&Count, sizeof(uint32));
	if (ElemSize != sizeof(T) || Count < 0)
		return false;
	if (Count > 0)
	{
		values.resize(Count);
		ifile.read((char*)values.data(), ElemSize * Count);
	}
}

template<typename T>
void WriteData(std::ofstream& ofile, const T& value)
{
	constexpr uint32 ElemSize = sizeof(T);
	constexpr uint32 Count = 1;
	ofile.write((const char*)&ElemSize, sizeof(uint32));
	ofile.write((const char*)&Count, sizeof(uint32));
	ofile.write((const char*)&value, ElemSize * Count);
}

template<typename T>
void WriteData(std::ofstream& ofile, const std::vector<T>& values)
{
	constexpr uint32 ElemSize = sizeof(T);
	uint32 Count = values.size();
	ofile.write((const char*)&ElemSize, sizeof(uint32));
	ofile.write((const char*)&Count, sizeof(uint32));
	if (Count > 0)
		ofile.write((const char*)values.data(), ElemSize * Count);
}

}

const char* FShadercUtils::GetCacheDirectory()
{
	switch (GetRenderAPI())
	{
	case Rev::ERenderAPI::Vulkan:
		return "Cache/Shaders/Vulkan";
	default:
		REV_CORE_ASSERT(0, "Unknow Render API");
		break;
	}
	return nullptr;
}

void FShadercUtils::CreateCacheDirectory()
{
	fs::path CacheDir(GetCacheDirectory());
	if (!fs::exists(CacheDir))
		fs::create_directories(CacheDir);
}

const char* FShadercUtils::GetCacheExtension()
{
	return ".rscached";
}

ERHIShaderStage FShadercUtils::StringToShaderStage(std::string_view InStr)
{
	if (InStr == "vertex")
		return ERHIShaderStage::Vertex;
	else if (InStr == "fragment" || InStr == "pixel")
		return ERHIShaderStage::Pixel;
	else if (InStr == "tesscontrol" || InStr == "hull")
		return ERHIShaderStage::Hull;
	else if (InStr == "tesseval" || InStr == "domain")
		return ERHIShaderStage::Domain;
	else if (InStr == "geometry")
		return ERHIShaderStage::Geometry;
	else if (InStr == "compute")
		return ERHIShaderStage::Compute;

	return ERHIShaderStage::Unknown;
}

const char* FShadercUtils::ShaderStageToString(ERHIShaderStage InStage)
{
	switch (InStage)
	{
	case ERHIShaderStage::Vertex:		return "Vertex";
	case ERHIShaderStage::Pixel:		return "Pixel";
	case ERHIShaderStage::Hull:			return "Hull";
	case ERHIShaderStage::Domain:		return "Domain";
	case ERHIShaderStage::Geometry:		return "Geometry";
	case ERHIShaderStage::Compute:		return "Compute";
	}
	REV_CORE_ASSERT(false);
	return nullptr;
}

FShadercSource FShadercUtils::LoadShaderSource(const FPath& InPath)
{
	FShadercSource Result;
	Result.FilePath = InPath;
	Result.FileContent = FFileSystem::LoadBinaryFile(InPath);
	/*if(Result.FileContent.Empty())
		return Result;

	std::string_view SrcStr(Result.FileContent.DataAs<char>(), Result.FileContent.Size());
	const std::regex re(R"(#pragma\s+shader_stage\((.*?)\))");

	using MatchResult = std::match_results<std::string_view::const_iterator>;
	MatchResult MatchRes;

	if (std::regex_search(SrcStr.begin(), SrcStr.end(), MatchRes, re))
	{
		REV_CORE_ASSERT(MatchRes.size() == 2, "Syntax error");
		const auto& SubRes = MatchRes[1];
		size_t StageBegin = SubRes.first - SrcStr.begin();
		const std::string_view StageStr(SrcStr.data() + StageBegin, SubRes.length());
		Result.Stage = StringToShaderStage(StageStr);
	}
	else
	{
		REV_CORE_ASSERT(false, "Unknow shader source stage");
	}*/

	return Result;
}

bool FShadercUtils::LoadShaderCompiledData(const std::filesystem::path& ShaderCachePath, FShadercCompiledData& OutCompiledData)
{
	if (fs::exists(ShaderCachePath))
	{
		Clock timer;
		{
			FFileArchive Ar(ShaderCachePath.generic_string(), EFileArchiveKind::Read);
			Ar << OutCompiledData;
		}
		if (!OutCompiledData.Binary.Empty())
		{
			REV_CORE_INFO("Shader '{0}' read from cache took {1} ms", OutCompiledData.Name.c_str(), timer.ElapsedMillis());
		}
		else
		{
			REV_CORE_ERROR("Shader '{0}' read from cache failed", OutCompiledData.Name.c_str());
		}
		return !OutCompiledData.Binary.Empty();
	}
	return false;
}

bool FShadercUtils::SaveShaderCompiledData(const std::filesystem::path& ShaderCachePath, FShadercCompiledData& InCompiledData)
{
	CreateCacheDirectory();
	fs::path CachedDir = ShaderCachePath.parent_path();
	if(!fs::exists(CachedDir))
		fs::create_directories(CachedDir);

	{
		FFileArchive Ar(ShaderCachePath.generic_string(), EFileArchiveKind::Write);
		Ar << InCompiledData;
	}

	return false;
}

void FShadercUtils::DumpShaderInfo(const FShadercCompiledData& InData)
{
	//for (auto&& [Stage, CompiledData] : InData.CompiledDataMap)
	{
		spirv_cross::Compiler Refl(InData.Binary.DataAs<uint32_t>(), InData.Binary.Size() / sizeof(uint32_t));
		spirv_cross::ShaderResources ResourceRefl = Refl.get_shader_resources();

		REV_CORE_TRACE("Shaderc::Reflect - {0} {1}", InData.Name.c_str(), ShaderStageToString(InData.Stage));
		REV_CORE_TRACE("    {0} uniform buffers", ResourceRefl.uniform_buffers.size());
		REV_CORE_TRACE("    {0} sampled images", ResourceRefl.sampled_images.size());
		//REV_CORE_TRACE("    {0} separate images", resources.separate_images.size());
		//REV_CORE_TRACE("    {0} separate samplers", resources.separate_samplers.size());

		//REV_CORE_TRACE("Uniform buffers:");
		//for (const auto& resource : resources.uniform_buffers)
		//{
		//	const auto& bufferType = compiler.get_type(resource.base_type_id);
		//	size_t bufferSize = compiler.get_declared_struct_size(bufferType);
		//	uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
		//	size_t memberCount = bufferType.member_types.size();

		//	REV_CORE_TRACE("  {0}", resource.name);
		//	REV_CORE_TRACE("    Size = {0}", bufferSize);
		//	REV_CORE_TRACE("    Binding = {0}", binding);
		//	REV_CORE_TRACE("    Members = {0}", memberCount);
		//}
	}
}

}