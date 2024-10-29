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

static FRHIShaderLibrary* sRHIShaderLibrary_Inst = nullptr;
static constexpr std::string_view sRHIShaderExtension = ".rsf";

void FRHIShaderLibrary::CreateInstance()
{
	sRHIShaderLibrary_Inst = new FRHIShaderLibrary;

	switch (GetRenderAPI())
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

void FRHIShaderLibrary::ReleaseInstance()
{
	delete sRHIShaderLibrary_Inst;
	sRHIShaderLibrary_Inst = nullptr;
}

FRHIShaderLibrary& FRHIShaderLibrary::GetInstance()
{
	REV_CORE_ASSERT(sRHIShaderLibrary_Inst != nullptr);
	return *sRHIShaderLibrary_Inst;
}

Ref<FRHIShaderProgram> FRHIShaderLibrary::CreateGraphicsProgram(const std::string& InProgramName, const FRHIShaderCreateDesc& InVertexDesc, const FRHIShaderCreateDesc& InFragmentDesc, const FRHIShaderCreateDesc& InTessControlDesc, const FRHIShaderCreateDesc& InTessEvalDesc, const FRHIShaderCreateDesc& InGeometryDesc)
{
	FRHIGraphicsShaders Shaders;
	Shaders.VertexShader = CreateShader(InVertexDesc, EShaderStage::Vertex);
	Shaders.PixelShader = CreateShader(InFragmentDesc, EShaderStage::Pixel);
	Shaders.HullShader = CreateShader(InTessControlDesc, EShaderStage::Hull);
	Shaders.DomainShader = CreateShader(InTessEvalDesc, EShaderStage::Domain);
	Shaders.GeometryShader = CreateShader(InGeometryDesc, EShaderStage::Geometry);

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