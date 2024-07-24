#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Core/Log.h"
#include "Rev/Core/Assert.h"
#include "Rev/Render/RenderCore.h"

#include "Platform/Shaderc/ShadercFactory.h"
//OpenGL
#include "Platform/OpenGL/OpenGLShaderFactory.h"

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
	RE_CORE_ASSERT(sRHIShaderLibrary_Inst != nullptr);
	return *sRHIShaderLibrary_Inst;
}

Ref<FRHIShaderProgram> FRHIShaderLibrary::CreateGraphicsProgram(const std::string& InProgramName, const FRHIShaderCreateDesc& InVertexDesc, const FRHIShaderCreateDesc& InFragmentDesc, const FRHIShaderCreateDesc& InTessControlDesc, const FRHIShaderCreateDesc& InTessEvalDesc, const FRHIShaderCreateDesc& InGeometryDesc)
{
	FRHIGraphicsShaders Shaders;
	Shaders.VertexShader = CreateShader(InVertexDesc);
	Shaders.PixelShader = CreateShader(InFragmentDesc);
	Shaders.HullShader = CreateShader(InTessControlDesc);
	Shaders.DomainShader = CreateShader(InTessEvalDesc);
	Shaders.GeometryShader = CreateShader(InGeometryDesc);

	switch (GetRenderAPI())
	{
	case ERenderAPI::OpenGL:
		return FOpenGLShaderFactory::CreateShaderProgram(InProgramName, Shaders);
	default:
		RE_CORE_ASSERT(false, "Unknown RenderAPI!");
		break;
	}
	return nullptr;
}

Ref<FRHIShader> FRHIShaderLibrary::LoadOrCompileShader(const FPath& InPath, const FRHIShaderCompileOptions& InOptions)
{
	auto CompiledData = FShadercFactory::LoadOrCompileShader(InPath, InOptions);
	if (CompiledData.Empty())
	{
		RE_CORE_WARN("No shader complied for {0}", InPath.FullPath().c_str());
		return {};
	}

	switch (GetRenderAPI())
	{
	case ERenderAPI::OpenGL:
	{
		auto pShader = FOpenGLShaderFactory::CreateShader(CompiledData);
		mShadersCache[CompiledData.Name].Add(InOptions.Hash(), pShader);
		return pShader;
	}
	default:
		RE_CORE_ASSERT(false, "Unknown RenderAPI!");
		break;
	}
	return {};
}

Ref<FRHIShader> FRHIShaderLibrary::CreateShader(const FRHIShaderCreateDesc& InDesc)
{
	if (InDesc.Name.empty()) return nullptr;
	Ref<FRHIShader> Result = nullptr;
	if (auto iter = mShadersCache.find(InDesc.Name); iter != mShadersCache.end())
	{
		Result = iter->second[InDesc.Options.Hash()];
	}
	if(Result == nullptr)
	{
		Result = LoadOrCompileShader(InDesc.Name + std::string(sRHIShaderExtension), InDesc.Options);
	}
	return Result;
}

void FRHIShaderLibrary::ClearShadersCache()
{
	mShadersCache.clear();
}

}