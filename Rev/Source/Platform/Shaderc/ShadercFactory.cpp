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

	if (!FShadercUtils::LoadShaderCompiledData(InPath, Result))
	{
		auto ShaderSource = FShadercUtils::LoadShaderSource(InPath);
		CompileShaders(ShaderSource, Result);
		FShadercUtils::SaveShaderCompiledData(InPath, Result);
	}

#ifdef RE_DEBUG
	FShadercUtils::DumpShaderInfo(Result);
#endif

	return Result;
}

}


