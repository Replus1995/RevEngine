#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Core/Log.h"
#include "Rev/Core/Assert.h"
#include "Rev/Render/RenderCore.h"

//Private
#include "Shaderc/ShadercFactory.h"
#include "VulkanRHI/VulkanShader.h"

namespace Rev
{

FShaderCompileConfig GShaderCompileConfig;

static constexpr std::string_view sRHIShaderExtension = ".rsf";
FRHIShaderLibrary* FRHIShaderLibrary::ShaderLibrary = nullptr;

void FRHIShaderLibrary::Initialize(ERenderAPI InAPI)
{
	if(ShaderLibrary != nullptr)
		return;

	ShaderLibrary = new FRHIShaderLibrary;
	switch (InAPI)
	{
	case ERenderAPI::Vulkan:
		GShaderCompileConfig.BufferOffset = 0;
		GShaderCompileConfig.SamplerOffset = 16;
		GShaderCompileConfig.TextureOffset = 32;
		break;
	default:
		break;
	}
}

void FRHIShaderLibrary::Shutdown()
{
	if(ShaderLibrary == nullptr)
		return;

	SAFE_DELETE(ShaderLibrary)
}

Ref<FRHIShaderProgram> FRHIShaderLibrary::CreateGraphicsProgram(const std::string& InProgramName, const FRHIShaderCreateDesc& InVertexDesc, const FRHIShaderCreateDesc& InFragmentDesc, const FRHIShaderCreateDesc& InTessControlDesc, const FRHIShaderCreateDesc& InTessEvalDesc, const FRHIShaderCreateDesc& InGeometryDesc)
{
	FRHIGraphicsShaders Shaders;
	Shaders.VertexShader = CreateShader(InVertexDesc, SS_Vertex);
	Shaders.PixelShader = CreateShader(InFragmentDesc,SS_Pixel);
	Shaders.HullShader = CreateShader(InTessControlDesc,SS_Hull);
	Shaders.DomainShader = CreateShader(InTessEvalDesc,SS_Domain);
	Shaders.GeometryShader = CreateShader(InGeometryDesc,SS_Geometry);

	switch (GetRenderAPI())
	{
	case ERenderAPI::Vulkan:
		return CreateRef<FVulkanShaderProgram>(InProgramName, Shaders);
	default:
		REV_CORE_ASSERT(false, "Unknown RenderAPI!");
		break;
	}
	return nullptr;
}

Ref<FRHIShader> FRHIShaderLibrary::LoadOrCompileShader(const FPath& InPath, const FRHIShaderCompileOptions& InOptions, EShaderStage InStage)
{
	auto CompiledData = FShadercFactory::LoadOrCompileShader(InPath, InOptions, InStage);
	if (CompiledData.Empty())
	{
		REV_CORE_WARN("No shader complied for {0}", InPath.ToString().c_str());
		return {};
	}

	switch (GetRenderAPI())
	{
	case ERenderAPI::Vulkan:
	{
		auto pShader = CreateRef<FVulkanShader>(CompiledData);
		mShadersCache[CompiledData.Name].Add(InOptions.GetHash(), pShader);
		return pShader;
	}
	default:
		REV_CORE_ASSERT(false, "Unknown RenderAPI!");
		break;
	}
	return {};
}

Ref<FRHIShader> FRHIShaderLibrary::CreateShader(const FRHIShaderCreateDesc& InDesc, EShaderStage InStage)
{
	if (InDesc.Name.empty()) return nullptr;
	Ref<FRHIShader> Result = nullptr;
	if (auto iter = mShadersCache.find(InDesc.Name); iter != mShadersCache.end())
	{
		Result = iter->second[InDesc.Options.GetHash()];
	}
	if(Result == nullptr)
	{
		Result = LoadOrCompileShader(InDesc.Name + std::string(sRHIShaderExtension), InDesc.Options, InStage);
	}
	return Result;
}

void FRHIShaderLibrary::ClearShadersCache()
{
	mShadersCache.clear();
}

}