#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Core/Log.h"
#include "Rev/Core/Assert.h"
#include "Rev/Render/RenderCore.h"

//Private
#include "Shaderc/ShadercFactory.h"
#include "OpenGL/OpenGLShader.h"
#include "VulkanRHI/VulkanShader.h"

namespace Rev
{

static FRHIShaderLibrary* sRHIShaderLibrary_Inst = nullptr;
static constexpr std::string_view sRHIShaderExtension = ".rsf";

void FRHIShaderLibrary::CreateInstance()
{
	sRHIShaderLibrary_Inst = new FRHIShaderLibrary;
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
	Shaders.VertexShader = CreateShader(InVertexDesc, ERHIShaderStage::Vertex);
	Shaders.PixelShader = CreateShader(InFragmentDesc, ERHIShaderStage::Pixel);
	Shaders.HullShader = CreateShader(InTessControlDesc, ERHIShaderStage::Hull);
	Shaders.DomainShader = CreateShader(InTessEvalDesc, ERHIShaderStage::Domain);
	Shaders.GeometryShader = CreateShader(InGeometryDesc, ERHIShaderStage::Geometry);

	switch (GetRenderAPI())
	{
	case ERenderAPI::OpenGL:
		return CreateRef<FOpenGLShaderProgram>(InProgramName, Shaders);
	case ERenderAPI::Vulkan:
		return CreateRef<FVulkanShaderProgram>(InProgramName, Shaders);
	default:
		REV_CORE_ASSERT(false, "Unknown RenderAPI!");
		break;
	}
	return nullptr;
}

Ref<FRHIShader> FRHIShaderLibrary::LoadOrCompileShader(const FPath& InPath, const FRHIShaderCompileOptions& InOptions, ERHIShaderStage InStage)
{
	auto CompiledData = FShadercFactory::LoadOrCompileShader(InPath, InOptions, InStage);
	if (CompiledData.Empty())
	{
		REV_CORE_WARN("No shader complied for {0}", InPath.ToString().c_str());
		return {};
	}

	switch (GetRenderAPI())
	{
	case ERenderAPI::OpenGL:
	{
		auto pShader = CreateRef<FOpenGLShader>(CompiledData.Stage, CompiledData.Binary);
		mShadersCache[CompiledData.Name].Add(InOptions.GetHash(), pShader);
		return pShader;
	}
	case ERenderAPI::Vulkan:
	{
		auto pShader = CreateRef<FVulkanShader>(CompiledData.Stage, CompiledData.Binary);
		mShadersCache[CompiledData.Name].Add(InOptions.GetHash(), pShader);
		return pShader;
	}
	default:
		REV_CORE_ASSERT(false, "Unknown RenderAPI!");
		break;
	}
	return {};
}

Ref<FRHIShader> FRHIShaderLibrary::CreateShader(const FRHIShaderCreateDesc& InDesc, ERHIShaderStage InStage)
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