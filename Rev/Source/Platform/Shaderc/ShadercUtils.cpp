#include "ShadercUtils.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Log.h"
#include "Rev/Core/Clock.h"

#include <vector>
#include <filesystem>
#include <spirv_cross/spirv_cross.hpp>

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

static const char* sShaderCachaExtension = ".shaderc_cached";

const char* FShadercUtils::GetCacheDirectory()
{
	switch (GetRenderAPI())
	{
	case Rev::ERenderAPI::OpenGL:
		return "ShaderCache/OpenGL";
	case Rev::ERenderAPI::Vulkan:
		return "ShaderCache/Vulkan";
	default:
		RE_CORE_ASSERT(0, "Unknow Render API");
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

ERHIShaderStage FShadercUtils::StringToShaderStage(std::string_view InStr)
{
	if (InStr == "vertex")
		return ERHIShaderStage::Vertex;
	else if (InStr == "fragment" || InStr == "pixel")
		return ERHIShaderStage::Pixel;
	else if (InStr == "geometry" )
		return ERHIShaderStage::Geometry;
	else if (InStr == "tess_control" || InStr == "hull")
		return ERHIShaderStage::Hull;
	else if (InStr == "tess_evaluation" || InStr == "domain")
		return ERHIShaderStage::Domain;
	else if (InStr == "compute" || InStr == "domain")
		return ERHIShaderStage::Domain;

	return ERHIShaderStage::Unknown;
}

const char* FShadercUtils::ShaderStageToString(ERHIShaderStage InStage)
{
	switch (InStage)
	{
	case ERHIShaderStage::Vertex:   return "Vertex";
	case ERHIShaderStage::Hull:		return "Hull";
	case ERHIShaderStage::Domain:   return "Domain";
	case ERHIShaderStage::Pixel:	return "Pixel";
	case ERHIShaderStage::Geometry: return "Geometry";
	case ERHIShaderStage::Compute:	return "Compute";
	}
	RE_CORE_ASSERT(false);
	return nullptr;
}

FShadercSource FShadercUtils::LoadShaderSource(const FPath& InPath)
{
	FShadercSource Result;
	Result.FilePath = InPath;
	Result.FileContent = FFileSystem::LoadBinaryFile(InPath);
	if(Result.FileContent.Empty())
		return Result;

	std::string_view SrcStr(Result.FileContent.DataAs<char>(), Result.FileContent.Size());
	std::string_view TypeToken = "#type";
	size_t pos = SrcStr.find(TypeToken, 0);
	while (pos != std::string_view::npos)
	{
		size_t eol = SrcStr.find_first_of("\r\n", pos); //End of shader type declaration line
		RE_CORE_ASSERT(eol != std::string_view::npos, "Syntax error");
		size_t begin = pos + TypeToken.length() + 1; //Start of shader type name (after "#type " keyword)

		std::string_view TypeStr = SrcStr.substr(begin, eol - begin);
		ERHIShaderStage Stage = StringToShaderStage(TypeStr);
		RE_CORE_ASSERT(Stage != ERHIShaderStage::Unknown, "Invalid shader type specified");

		size_t nextLinePos = SrcStr.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
		RE_CORE_ASSERT(nextLinePos != std::string_view::npos, "Syntax error");
		pos = SrcStr.find(TypeToken, nextLinePos); //Start of next shader type declaration line

		size_t StageContentSize = pos == std::string_view::npos ? Result.FileContent.Size() - nextLinePos : pos - nextLinePos;
		FBufferView StageContent(Result.FileContent, nextLinePos, StageContentSize);
		Result.StageSourceMap.emplace(Stage, std::move(StageContent));
	}

	return Result;
}

bool FShadercUtils::LoadShaderCompiledData(const FPath& ShaderPath, FShadercCompiledData& OutCompiledData)
{
	fs::path CachedPath(GetCacheDirectory() + ShaderPath.String() + sShaderCachaExtension);
	if (fs::exists(CachedPath))
	{
		std::ifstream InFile(CachedPath, std::ios::in | std::ios::binary);
		if (InFile.is_open())
		{
			Clock timer;
			OutCompiledData.CompiledDataMap.clear();
			bool ReadSuccess = true;
			while (InFile.peek() != EOF)
			{
				ERHIShaderStage Stage = ERHIShaderStage::Unknown;
				std::vector<uint32_t> CompiledData;
				if(ReadSuccess = ReadData(InFile, Stage); !ReadSuccess)
					break;
				if (ReadSuccess = ReadData(InFile, CompiledData); !ReadSuccess)
					break;
				OutCompiledData.CompiledDataMap.emplace(Stage, std::move(CompiledData));
			}
			if(ReadSuccess)
				RE_CORE_INFO("Shader '{0}' read from cache took {1} ms", OutCompiledData.Name.c_str(), timer.ElapsedMillis());
			else
			{
				RE_CORE_ERROR("Shader '{0}' cache corrupted", OutCompiledData.Name.c_str());
				OutCompiledData.CompiledDataMap.clear();
			}
			return ReadSuccess;
		}
		else
		{
			RE_CORE_ERROR("[FShadercUtils] Open file failded '{0}'", CachedPath.string().c_str());
		}
	}
	return false;
}

bool FShadercUtils::SaveShaderCompiledData(const FPath& ShaderPath, const FShadercCompiledData& InCompiledData)
{
	CreateCacheDirectory();

	fs::path CachedPath(GetCacheDirectory() + ShaderPath.String() + sShaderCachaExtension);
	fs::path CachedDir = CachedPath.parent_path();
	if(!fs::exists(CachedDir))
		fs::create_directories(CachedDir);

	std::ofstream OutFile(CachedPath, std::ios::out | std::ios::binary);
	if (OutFile.is_open())
	{
		for (auto&[Stage, CompiledData] : InCompiledData.CompiledDataMap)
		{
			WriteData(OutFile, Stage);
			WriteData(OutFile, CompiledData);
		}
		return true;
	}
	else
	{
		RE_CORE_ERROR("[FShadercUtils] Open file failded '{0}'", CachedPath.string().c_str());
	}

	return false;
}

void FShadercUtils::DumpShaderInfo(const FShadercCompiledData& InData)
{
	for (auto&& [Stage, CompiledData] : InData.CompiledDataMap)
	{
		spirv_cross::Compiler compiler(CompiledData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		RE_CORE_TRACE("Shaderc::Reflect - {0} {1}", InData.Name.c_str(), ShaderStageToString(Stage));
		RE_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
		RE_CORE_TRACE("    {0} resources", resources.sampled_images.size());

		RE_CORE_TRACE("Uniform buffers:");
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			size_t bufferSize = compiler.get_declared_struct_size(bufferType);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			size_t memberCount = bufferType.member_types.size();

			RE_CORE_TRACE("  {0}", resource.name);
			RE_CORE_TRACE("    Size = {0}", bufferSize);
			RE_CORE_TRACE("    Binding = {0}", binding);
			RE_CORE_TRACE("    Members = {0}", memberCount);
		}
	}
}

}