#include "ShadercFactory.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Clock.h"
#include "Rev/Core/Log.h"
#include <filesystem>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_glsl.hpp>

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
	RE_CORE_ASSERT(!HeaderContent.Empty(), "ShaderIncluder::GetInclude header file load failed.");

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
	case ERHIShaderStage::Vertex:   return shaderc_glsl_vertex_shader;
	case ERHIShaderStage::Hull:		return shaderc_glsl_tess_control_shader;
	case ERHIShaderStage::Domain:   return shaderc_glsl_tess_evaluation_shader;
	case ERHIShaderStage::Pixel:	return shaderc_glsl_fragment_shader;
	case ERHIShaderStage::Geometry: return shaderc_glsl_geometry_shader;
	case ERHIShaderStage::Compute:	return shaderc_glsl_compute_shader;
	}
	RE_CORE_ASSERT(false);
	return (shaderc_shader_kind)0;
}

static void InitCompileOptions(shaderc::CompileOptions& options)
{
	switch (GetRenderAPI())
	{
	case Rev::ERenderAPI::OpenGL:
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		break;
	case Rev::ERenderAPI::Vulkan:
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		break;
	default:
		RE_CORE_ASSERT(false, "Unknow Render API")
			break;
	}
	options.SetIncluder(CreateScope<ShaderIncluder>());

	constexpr bool bOptimize = false;
	if (bOptimize)
		options.SetOptimizationLevel(shaderc_optimization_level_performance);
}

void FShadercFactory::CompileShaders(const FShadercSource& InSource, FShadercCompiledData& OutData)
{
	Clock timer;
	OutData.CompiledDataMap.clear();
	std::string NativeFilePath = InSource.FilePath.ToNative();
	shaderc::Compiler compiler;
	shaderc::CompileOptions options;
	InitCompileOptions(options);

	for (auto&& [Stage, Content] : InSource.StageSourceMap)
	{
		shaderc::SpvCompilationResult CompileResult = compiler.CompileGlslToSpv(Content.DataAs<char>(), Content.Size(), ShaderStageToShadercKind(Stage), NativeFilePath.c_str(), options);
		if (CompileResult.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			RE_CORE_ERROR(CompileResult.GetErrorMessage());
			RE_CORE_ASSERT(false);
		}
		std::vector<uint32_t> CompileData(CompileResult.cbegin(), CompileResult.cend());
		OutData.CompiledDataMap.emplace(Stage, std::move(CompileData));
	}
	RE_CORE_INFO("Shader '{0}' compile took {1} ms", OutData.Name.c_str(), timer.ElapsedMillis());
}

FShadercCompiledData FShadercFactory::LoadAndCompile(const FPath& InPath)
{

	FShadercCompiledData Result;
	Result.Name = InPath.FullPath(false);

	fs::path ShaderCachePath(FShadercUtils::GetCacheDirectory() + InPath.FullPath(false) + FShadercUtils::GetCacheExtension());
	auto CacheWriteTime = fs::last_write_time(ShaderCachePath);
	auto SourceWriteTime = fs::last_write_time(InPath.ToNative());
	bool bNeedCompile = SourceWriteTime > CacheWriteTime;
	if (!bNeedCompile)
	{
		bNeedCompile = !FShadercUtils::LoadShaderCompiledData(ShaderCachePath, Result);
	}
	else
	{
		RE_CORE_INFO("Shader '{0}' cache out of date.", Result.Name.c_str());
	}

	if (bNeedCompile)
	{
		auto ShaderSource = FShadercUtils::LoadShaderSource(InPath);
		CompileShaders(ShaderSource, Result);
		FShadercUtils::SaveShaderCompiledData(ShaderCachePath, Result);
	}

#ifdef RE_DEBUG
	FShadercUtils::DumpShaderInfo(Result);
#endif

	return Result;
}

}


