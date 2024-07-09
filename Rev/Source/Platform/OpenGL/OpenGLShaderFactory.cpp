#include "OpenGLShaderFactory.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Clock.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace fs = std::filesystem;

namespace Rev
{

namespace
{

static const char* GetOpenGLStageStr(ERHIShaderStage stage)
{
	switch (stage)
	{
	case ERHIShaderStage::Vertex:   return "GL_VERTEX_SHADER";
	case ERHIShaderStage::Pixel: return "GL_FRAGMENT_SHADER";
	default: break;
	}
	RE_CORE_ASSERT(false);
	return nullptr;
}

static ERHIShaderStage GetShaderStage(const std::string& type)
{
	if (type == "vertex")
		return ERHIShaderStage::Vertex;
	else if (type == "fragment" || type == "pixel")
		return ERHIShaderStage::Pixel;

	return ERHIShaderStage::Unknown;
}

static shaderc_shader_kind GetShadercKind(ERHIShaderStage stage)
{
	switch (stage)
	{
	case ERHIShaderStage::Vertex:   return shaderc_glsl_vertex_shader;
	case ERHIShaderStage::Hull:		return shaderc_glsl_tess_control_shader;
	case ERHIShaderStage::Domain:   return shaderc_tess_evaluation_shader;
	case ERHIShaderStage::Pixel:	return shaderc_glsl_fragment_shader;
	case ERHIShaderStage::Geometry: return shaderc_glsl_geometry_shader;
	case ERHIShaderStage::Compute:	return shaderc_glsl_compute_shader;
	}
	RE_CORE_ASSERT(false);
	return (shaderc_shader_kind)0;
}

static std::vector<uint32_t> LoadCompiledData(const std::string& InPath)
{
	std::vector<uint32_t> OutVec;
	std::ifstream InFile(InPath, std::ios::in | std::ios::binary);
	if (InFile.is_open())
	{
		InFile.seekg(0, std::ios::end);
		auto ContentSize = InFile.tellg();
		InFile.seekg(0, std::ios::beg);

		if (ContentSize > 0)
		{
			OutVec.resize(ContentSize / sizeof(uint32_t));
			InFile.read((char*)OutVec.data(), ContentSize);
		}
	}
	else
	{
		RE_CORE_ERROR("[FFileSystem] Open file failded '{0}'", InPath.c_str());
	}
	return OutVec;
}

bool SaveCompiledData(const std::string& InPath, const std::vector<uint32_t>& InCompiledData)
{
	std::ofstream OutFile(InPath, std::ios::out | std::ios::binary);
	if (OutFile.is_open())
	{
		if (!InCompiledData.empty())
		{
			OutFile.write((char*)InCompiledData.data(), InCompiledData.size() * sizeof(uint32));
		}
		return true;
	}
	else
	{
		RE_CORE_ERROR("[FFileSystem] Open file failded '{0}'", InPath.c_str());
	}
	return false;
}

}


const char* FOpenGLShaderFactory::GetCacheDirectory()
{
	return "AssetCache/Shader/OpenGL";
}

const char* FOpenGLShaderFactory::GetCachedFileExtension(ERHIShaderStage InStage)
{
	switch (InStage)
	{
	case ERHIShaderStage::Vertex:
		return ".vertex.cached";
	case ERHIShaderStage::Hull:
		return ".hull.cached";
	case ERHIShaderStage::Domain:
		return ".domain.cached";
	case ERHIShaderStage::Pixel:
		return ".fragment.cached";
	case ERHIShaderStage::Geometry:
		return ".geometry.cached";
	case ERHIShaderStage::Compute:
		return ".compute.cached";
	default:
		break;
	}
	RE_CORE_ASSERT(false);
	return "";
}

void FOpenGLShaderFactory::CreateCacheDirectory()
{
	fs::path CacheDir(GetCacheDirectory());
	if (!fs::exists(CacheDir))
		fs::create_directories(CacheDir);
}

void FOpenGLShaderFactory::DumpShaderInfo(const std::string& InName, ERHIShaderStage InStage, const std::vector<uint32_t>& InData)
{
	spirv_cross::Compiler compiler(InData);
	spirv_cross::ShaderResources resources = compiler.get_shader_resources();

	RE_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", InName.c_str(), GetOpenGLStageStr(InStage));
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

FOpenGLCompiledShaderData FOpenGLShaderFactory::LoadAndCompile(const FPath& InPath, const std::string& InName)
{
	RE_CORE_ASSERT(!InName.empty());
	CreateCacheDirectory();

	FOpenGLCompiledShaderData Result;
	Result.Name = InName;

	fs::path CachedDir = fs::path(GetCacheDirectory() + InPath.String()).parent_path() / InPath.Name();
	if (fs::exists(CachedDir))
	{
		Clock timer;
		//Compiled, read from cache
		for (uint8 i = uint8(ERHIShaderStage::Vertex); i < uint8(ERHIShaderStage::Count); i++)
		{
			ERHIShaderStage Stage = ERHIShaderStage(i);
			fs::path CachedPath = CachedDir / (InPath.Name() + GetCachedFileExtension(Stage));
			if (fs::exists(CachedPath))
			{
				std::vector<uint32_t> CompileData = LoadCompiledData(CachedPath.generic_u8string());
				Result.CompiledDataMap.emplace(Stage, std::move(CompileData));
			}
		}
		RE_CORE_INFO("Shader '{0}' read from cache took {1} ms", InName.c_str(), timer.ElapsedMillis());
	}
	else
	{
		Clock timer;

		std::string NativePath = InPath.ToNative();
		std::string StrContent = FFileSystem::LoadStringFile(NativePath);
		if (StrContent.empty())
			return FOpenGLCompiledShaderData();

		std::map<ERHIShaderStage, std::string> ShaderStrSources;

		//Preprocess
		const char* TypeToken = "#type";
		size_t TypeTokenLength = strlen(TypeToken);
		size_t pos = StrContent.find(TypeToken, 0); //Start of shader type declaration line
		while (pos != std::string::npos)
		{
			size_t eol = StrContent.find_first_of("\r\n", pos); //End of shader type declaration line
			RE_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + TypeTokenLength + 1; //Start of shader type name (after "#type " keyword)
			std::string type = StrContent.substr(begin, eol - begin);
			ERHIShaderStage stage = GetShaderStage(type);
			RE_CORE_ASSERT(stage != ERHIShaderStage::Unknown, "Invalid shader type specified");

			size_t nextLinePos = StrContent.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
			RE_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = StrContent.find(TypeToken, nextLinePos); //Start of next shader type declaration line

			ShaderStrSources[stage] = (pos == std::string::npos) ? StrContent.substr(nextLinePos) : StrContent.substr(nextLinePos, pos - nextLinePos);
		}
		//Preprocess

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		const bool optimize = false;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		for (auto&& [Stage, StrSource] : ShaderStrSources)
		{
			shaderc::SpvCompilationResult CompileResult = compiler.CompileGlslToSpv(StrSource, GetShadercKind(Stage), NativePath.c_str(), options);
			if (CompileResult.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				RE_CORE_ERROR(CompileResult.GetErrorMessage());
				RE_CORE_ASSERT(false);
			}
			std::vector<uint32_t> CompileData(CompileResult.cbegin(), CompileResult.cend());
			fs::path CachedPath = CachedDir / (InPath.Name() + GetCachedFileExtension(Stage));
			//SaveCompiledData(CachedPath.generic_u8string(), CompileData);

			Result.CompiledDataMap.emplace(Stage, std::move(CompileData));
		}

		RE_CORE_INFO("Shader '{0}' compile took {1} ms", InName.c_str(), timer.ElapsedMillis());
	}

#ifdef RE_DEBUG
	for (auto&& [Stage, CompiledData] : Result.CompiledDataMap)
		DumpShaderInfo(Result.Name, Stage, CompiledData);
#endif
	return Result;
}

FRHIGraphicsShaders FOpenGLShaderFactory::CreateGraphicsShaders(const FOpenGLCompiledShaderData& InData)
{
	FRHIGraphicsShaders Result;	for (auto&[Stage, CompiledData] : InData.CompiledDataMap)
	{
		switch (Stage)
		{
		case ERHIShaderStage::Vertex:
			Result.VertexShader = CreateRef<FOpenGLShader>(Stage, CompiledData);
			break;
		case ERHIShaderStage::Hull:
			Result.HullShader = CreateRef<FOpenGLShader>(Stage, CompiledData);
			break;
		case ERHIShaderStage::Domain:
			Result.DomainShader = CreateRef<FOpenGLShader>(Stage, CompiledData);
			break;
		case ERHIShaderStage::Pixel:
			Result.PixelShader = CreateRef<FOpenGLShader>(Stage, CompiledData);
			break;
		case ERHIShaderStage::Geometry:
			Result.DomainShader = CreateRef<FOpenGLShader>(Stage, CompiledData);
			break;
		default:
			break;
		}
	}
	return Result;
}

Ref<FOpenGLShaderProgram> FOpenGLShaderFactory::CreateShaderProgram(const std::string& InName, const FRHIGraphicsShaders& InShaders)
{
	return CreateRef<FOpenGLShaderProgram>(InName, InShaders);
}

}