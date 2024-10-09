#include "ShadercFactory.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Clock.h"
#include "Rev/Core/Log.h"
#include "Rev/Render/RHI/RHITexture.h"
#include <filesystem>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include <spirv_cross/spirv_reflect.hpp>

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

constexpr uint8_t kSpirvSamplerShift = 16;

static EPixelFormat sSpirvFormatMapping[] =
{
	PF_Unknown,					// spv::ImageFormatUnknown = 0
	PF_R32G32B32A32_FLOAT,		// spv::ImageFormatRgba32f = 1
	PF_R16G16B16A16_FLOAT,		// spv::ImageFormatRgba16f = 2
	PF_R32_FLOAT,				// spv::ImageFormatR32f = 3
	PF_R8G8B8A8,				// spv::ImageFormatRgba8 = 4
	PF_R8G8B8A8_SNORM,			// spv::ImageFormatRgba8Snorm = 5
	PF_R32G32_FLOAT,			// spv::ImageFormatRg32f = 6
	PF_R16G16_FLOAT,			// spv::ImageFormatRg16f = 7
	PF_R11G11B10_FLOAT,			// spv::ImageFormatR11fG11fB10f = 8
	PF_R16_FLOAT,				// spv::ImageFormatR16f = 9
	PF_R16G16B16A16,			// spv::ImageFormatRgba16 = 10
	PF_R10G10B10A2,				// spv::ImageFormatRgb10A2 = 11
	PF_R16G16,					// spv::ImageFormatRg16 = 12
	PF_R8G8,					// spv::ImageFormatRg8 = 13
	PF_R16,						// spv::ImageFormatR16 = 14
	PF_R8,						// spv::ImageFormatR8 = 15
	PF_R16G16B16A16_SNORM,		// spv::ImageFormatRgba16Snorm = 16
	PF_R16G16_SNORM,			// spv::ImageFormatRg16Snorm = 17
	PF_R8G8_SNORM,				// spv::ImageFormatRg8Snorm = 18
	PF_R16_SNORM,				// spv::ImageFormatR16Snorm = 19
	PF_R8_SNORM,				// spv::ImageFormatR8Snorm = 20
	PF_R32G32B32A32_SINT,		// spv::ImageFormatRgba32i = 21
	PF_R16G16B16A16_SINT,		// spv::ImageFormatRgba16i = 22
	PF_R8G8B8A8_SINT,			// spv::ImageFormatRgba8i = 23
	PF_R32_SINT,				// spv::ImageFormatR32i = 24
	PF_R32G32_SINT,				// spv::ImageFormatRg32i = 25
	PF_R16G16_SINT,				// spv::ImageFormatRg16i = 26
	PF_R8G8_SINT,				// spv::ImageFormatRg8i = 27
	PF_R16_SINT,				// spv::ImageFormatR16i = 28
	PF_R8_SINT,					// spv::ImageFormatR8i = 29
	PF_R32G32B32A32_UINT,		// spv::ImageFormatRgba32ui = 30
	PF_R16G16B16A16_UINT,		// spv::ImageFormatRgba16ui = 31
	PF_R8G8B8A8_UINT,			// spv::ImageFormatRgba8ui = 32
	PF_R32_UINT,				// spv::ImageFormatR32ui = 33
	PF_Unknown,					// spv::ImageFormatRgb10a2ui = 34
	PF_R32G32_UINT,				// spv::ImageFormatRg32ui = 35
	PF_R16G16_UINT,				// spv::ImageFormatRg16ui = 36
	PF_R8G8_UINT,				// spv::ImageFormatRg8ui = 37
	PF_R16_UINT,				// spv::ImageFormatR16ui = 38
	PF_R8_UINT,					// spv::ImageFormatR8ui = 39
	PF_Unknown,					// spv::ImageFormatR64ui = 40
	PF_Unknown,					// spv::ImageFormatR64i = 41
};

ETextureDimension TranslateTextureDimension(spv::Dim InDim, bool bArray)
{
	switch (InDim)
	{
	case spv::Dim::Dim2D:
		return bArray ? ETextureDimension::Texture2DArray : ETextureDimension::Texture2D;
	case spv::Dim::Dim3D:
		return ETextureDimension::Texture3D;
	case spv::Dim::DimCube:
		return bArray ? ETextureDimension::TextureCubeArray : ETextureDimension::TextureCube;
	default:
		REV_CORE_ASSERT(false, "Unknown texture dimension");
		return ETextureDimension::Texture2D;
	}
}


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

void FShadercFactory::ReflectUniformInfo(FShadercCompiledData& Data)
{
	spirv_cross::CompilerReflection Refl(Data.Binary.DataAs<uint32_t>(), Data.Binary.Size() / sizeof(uint32_t));
	spirv_cross::ShaderResources ResourceRefl = Refl.get_shader_resources();

#ifdef REV_DEBUG
	REV_CORE_TRACE("Shaderc::Reflect - {0} {1}", Data.Name.c_str(), FShadercUtils::ShaderStageToString(Data.Stage));
	REV_CORE_TRACE("Buffers:");
#endif

	for (auto& Res : ResourceRefl.uniform_buffers)
	{
		const auto& BufferType = Refl.get_type(Res.base_type_id);
		size_t BufferSize = Refl.get_declared_struct_size(BufferType);
		size_t MemberCount = BufferType.member_types.size();

		FRHIUniformInfo Uniform;
		Uniform.Name = Res.name;
		Uniform.Type = ERHIUniformType::Buffer;
		Uniform.Num = 1;
		Uniform.Binding = Refl.get_decoration(Res.id, spv::Decoration::DecorationBinding);

		Data.Uniforms.push_back(Uniform);


#ifdef REV_DEBUG
		REV_CORE_TRACE("  {0}: Binding = {1}, Size = {2}, Members = {3}", Uniform.Name.c_str(), Uniform.Binding, BufferSize, MemberCount);
#endif
	}

#ifdef REV_DEBUG
	REV_CORE_TRACE("Images:");
#endif

	for (auto& Res : ResourceRefl.separate_images)
	{
		uint32 binding_index = Refl.get_decoration(Res.id, spv::Decoration::DecorationBinding);

		auto imageType = Refl.get_type(Res.base_type_id).image;
		auto componentType = Refl.get_type(imageType.type).basetype;

		bool isCompareSampler = false;
		for (auto& sampler : ResourceRefl.separate_samplers)
		{
			if (binding_index + kSpirvSamplerShift == Refl.get_decoration(sampler.id, spv::Decoration::DecorationBinding))
			{
				//std::string samplerName = Refl.get_name(sampler.id);
				isCompareSampler = Refl.variable_is_depth_or_compare(sampler.id);
				break;
			}
		}

		FRHIUniformInfo Uniform;
		Uniform.Name = Res.name;
		Uniform.Type = ERHIUniformType::Image;
		Uniform.Num = 1;
		Uniform.Binding = uint16(binding_index);

		Uniform.TexFormat = sSpirvFormatMapping[uint32(imageType.format)];
		Uniform.TexDimension = TranslateTextureDimension(imageType.dim, imageType.arrayed);

#ifdef REV_DEBUG
		REV_CORE_TRACE(" {0}: Binding = {1}", Uniform.Name.c_str(), Uniform.Binding);
#endif

		Data.Uniforms.push_back(Uniform);
	}

}

void FShadercFactory::CompileShaders(const FShadercSource& InSource, const FRHIShaderCompileOptions& InOptions, FShadercCompiledData& OutData)
{
	Clock timer;
	std::string NativeFilePath = InSource.FilePath.ToNative();
	shaderc::Compiler compiler;
	shaderc::CompileOptions options;
	InitCompileOptions(options);
	AddCompileMacros(options, InOptions.Macros);

	shaderc_shader_kind kind = ShaderStageToShadercKind(InSource.Stage);

	{
		/*shaderc::PreprocessedSourceCompilationResult PreprocessResult = compiler.PreprocessGlsl(InSource.FileContent.DataAs<char>(), kind, NativeFilePath.c_str(), options);
		if (PreprocessResult.GetCompilationStatus() != shaderc_compilation_status_success) {
			REV_CORE_ERROR(PreprocessResult.GetErrorMessage());
			REV_CORE_ASSERT(false);
		}*/

		shaderc::SpvCompilationResult CompileResult = compiler.CompileGlslToSpv(InSource.FileContent.DataAs<char>(), InSource.FileContent.Size(), kind, NativeFilePath.c_str(), options);
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
		ReflectUniformInfo(Result);
		//FShadercUtils::SaveShaderCompiledData(ShaderCachePath, Result);
	}

	return Result;
}

ERHIShaderStage FShadercFactory::DetectShaderStage(const FShadercSource& InSource)
{
	//TODO
	return ERHIShaderStage::Compute;
}

}


