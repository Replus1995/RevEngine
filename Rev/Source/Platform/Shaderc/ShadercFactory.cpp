#include "ShadercFactory.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Clock.h"
#include "Rev/Core/Log.h"
#include <filesystem>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_glsl.hpp>

/*
[[shader-stage-selection]]
.Shader Stage Selection
|===
|Shader Stage |Shader File Extension |`<stage>`
|vertex                 |`.vert` |`vertex`
|fragment               |`.frag` |`fragment`
|tesselation control    |`.tesc` |`tesscontrol`
|tesselation evaluation |`.tese` |`tesseval`
|geometry               |`.geom` |`geometry`
|compute                |`.comp` |`compute`
|===
*/

namespace fs = std::filesystem;

namespace Rev
{

class ShaderIncluder : public shaderc::CompileOptions::IncluderInterface
{
public:
	virtual shaderc_include_result* GetInclude(
		const char* requested_source,
		shaderc_include_type type,
		const char* requesting_source,
		size_t include_depth
	) override;
	void ReleaseInclude(shaderc_include_result* data) override;

	struct HeaderContainer
	{
		std::string Name;
		FBuffer Content;
	};
};

shaderc_include_result* ShaderIncluder::GetInclude(
	const char* requested_source,
	shaderc_include_type type,
	const char* requesting_source,
	size_t include_depth
)
{
	std::string HeaderPath(requested_source);
	FBuffer HeaderContent = FFileSystem::LoadBinaryFile(FPath(HeaderPath));
	REV_CORE_ASSERT(!HeaderContent.Empty(), "ShaderIncluder::GetInclude header file load failed.");

	auto Container = new HeaderContainer;
	Container->Name = std::move(HeaderPath);
	Container->Content = std::move(HeaderContent);

	auto Result = new shaderc_include_result;
	Result->user_data = Container;
	Result->source_name = Container->Name.data();
	Result->source_name_length = Container->Name.length();
	Result->content = Container->Content.DataAs<char>();
	Result->content_length = Container->Content.Size();

	return Result;
}

void ShaderIncluder::ReleaseInclude(shaderc_include_result* data)
{
	delete static_cast<HeaderContainer*>(data->user_data);
	delete data;
}

static shaderc_shader_kind ShaderStageToShadercKind(ERHIShaderStage InStage)
{
	switch (InStage)
	{
	case ERHIShaderStage::Vertex:		return shaderc_vertex_shader;
	case ERHIShaderStage::Hull:			return shaderc_tess_control_shader;
	case ERHIShaderStage::Domain:		return shaderc_tess_evaluation_shader;
	case ERHIShaderStage::Pixel:		return shaderc_fragment_shader;
	case ERHIShaderStage::Geometry:		return shaderc_geometry_shader;
	case ERHIShaderStage::Compute:		return shaderc_compute_shader;
	}
	REV_CORE_ASSERT(false, "Unknown Shader Stage");
	return (shaderc_shader_kind)0;
}

static void InitCompileOptions(shaderc::CompileOptions& Options)
{
	switch (GetRenderAPI())
	{
	case Rev::ERenderAPI::OpenGL:
		Options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		break;
	case Rev::ERenderAPI::Vulkan:
		Options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
		break;
	default:
		REV_CORE_ASSERT(false, "Unknow Render API")
			break;
	}
	Options.SetSourceLanguage(shaderc_source_language_hlsl);
	Options.SetIncluder(CreateScope<ShaderIncluder>());

	//Todo: shader limits

	constexpr bool bOptimize = false;
	if (bOptimize)
		Options.SetOptimizationLevel(shaderc_optimization_level_performance);
}

static void AddCompileMacros(shaderc::CompileOptions& Options, const std::string& InMacros)
{
	//todo
}

void FShadercFactory::CompileShaders(const FShadercSource& InSource, const FRHIShaderCompileOptions& InOptions, FShadercCompiledData& OutData)
{
	Clock timer;
	std::string NativeFilePath = InSource.FilePath.ToNative();
	shaderc::Compiler compiler;
	shaderc::CompileOptions options;
	InitCompileOptions(options);
	AddCompileMacros(options, InOptions.Macros);

	{
		shaderc::SpvCompilationResult CompileResult = compiler.CompileGlslToSpv(InSource.FileContent.DataAs<char>(), InSource.FileContent.Size(), ShaderStageToShadercKind(InSource.Stage), NativeFilePath.c_str(), options);
		if (CompileResult.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			REV_CORE_ERROR(CompileResult.GetErrorMessage());
			REV_CORE_ASSERT(false);
		}
		OutData.Stage = InSource.Stage;
		size_t CompiledSize = CompileResult.cend() - CompileResult.cbegin();
		OutData.Binary.Allocate(CompiledSize * sizeof(uint32_t));
		memcpy(OutData.Binary.Data(), CompileResult.cbegin(), CompiledSize * sizeof(uint32_t));
	}
	REV_CORE_INFO("Shader '{0}' compile took {1} ms", OutData.Name.c_str(), timer.ElapsedMillis());
}

FShadercCompiledData FShadercFactory::LoadOrCompileShader(const FPath& InPath, const FRHIShaderCompileOptions& InOptions, ERHIShaderStage InStage)
{
	FShadercCompiledData Result;
	Result.Name = InPath.ToString(false);

	std::string OptionHashStr = std::to_string(InOptions.GetHash());
	fs::path ShaderCachePath(FShadercUtils::GetCacheDirectory() + Result.Name + "_" + OptionHashStr + FShadercUtils::GetCacheExtension());
	bool bNeedCompile = true;
	/*if (fs::exists(ShaderCachePath))
	{
		auto CacheWriteTime = fs::last_write_time(ShaderCachePath);
		auto SourceWriteTime = fs::last_write_time(InPath.ToNative());
		if (SourceWriteTime > CacheWriteTime)
		{
			REV_CORE_INFO("Shader '{0}' cache out of date.", Result.Name.c_str());
		}
		else
		{
			bNeedCompile = !FShadercUtils::LoadShaderCompiledData(ShaderCachePath, Result);
		}
	}*/

	if (bNeedCompile)
	{
		auto ShaderSource = FShadercUtils::LoadShaderSource(InPath);
		ShaderSource.Stage = InStage != ERHIShaderStage::Unknown ? InStage : DetectShaderStage(ShaderSource);
		CompileShaders(ShaderSource, InOptions, Result);
		//FShadercUtils::SaveShaderCompiledData(ShaderCachePath, Result);
	}

#ifdef REV_DEBUG
	FShadercUtils::DumpShaderInfo(Result);
#endif

	return Result;
}

ERHIShaderStage FShadercFactory::DetectShaderStage(const FShadercSource& InSource)
{
	return ERHIShaderStage::Compute;
}

}


