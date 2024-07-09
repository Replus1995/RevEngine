#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Core/Assert.h"
#include "Rev/Render/RenderCore.h"

//OpenGL
#include "Platform/OpenGL/OpenGLShaderFactory.h"

namespace Rev
{

static FRHIShaderLibrary* sRHIShaderLibrary_Inst = nullptr;

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

void FRHIShaderLibrary::CompileShaderSource(const FPath& InPath, const std::string& InName)
{
	std::string FinalName = InName.empty() ? InPath.Name() : InName;
	switch (GetRenderAPI())
	{
	case ERenderAPI::OpenGL:
	{
		FOpenGLCompiledShaderData ShaderData = FOpenGLShaderFactory::LoadAndCompile(InPath, FinalName);
		FRHIGraphicsShaders GraphicsShaders = FOpenGLShaderFactory::CreateGraphicsShaders(ShaderData);
		mGraphicsShaderCache.emplace(ShaderData.Name, std::move(GraphicsShaders));
		break;
	}
	default:
		RE_CORE_ASSERT(false, "Unknown RenderAPI!");
		break;
	}

}

Ref<FRHIShader> FRHIShaderLibrary::FindShader(const std::string& InName, ERHIShaderStage InStage)
{
	if (InName.empty()) return nullptr;
	if (uint8(InStage) < uint8(ERHIShaderStage::Compute))
	{
		if (auto Iter = mGraphicsShaderCache.find(InName); Iter != mGraphicsShaderCache.end())
		{
			return Iter->second[InStage];
		}
	}
	return nullptr;
}

Ref<FRHIShaderProgram> FRHIShaderLibrary::CreateGraphicsProgram(
	const std::string& InProgramName,
	const std::string& InShadersName)
{
	Ref<FRHIShaderProgram> Program = FindProgram(InProgramName);
	if(Program)
		return Program;

	if (auto Iter = mGraphicsShaderCache.find(InShadersName); Iter != mGraphicsShaderCache.end())
	{
		auto& Shaders = Iter->second;
		switch (GetRenderAPI())
		{
		case ERenderAPI::OpenGL:
		{
			 Program = FOpenGLShaderFactory::CreateShaderProgram(InProgramName, Shaders);
			if (Program)
			{
				mShaderProgramCache.emplace(InProgramName, Program);
			}
			return Program;
		}
		default:
			RE_CORE_ASSERT(false, "Unknown RenderAPI!");
			break;
		}
	}
	return nullptr;
}

Ref<FRHIShaderProgram> FRHIShaderLibrary::CreateGraphicsProgram(
	const std::string& InProgramName, 
	const std::string& InVertexName, 
	const std::string& InPixelName, 
	const std::string& InHullName, 
	const std::string& InDomainName, 
	const std::string& InGeometryName)
{
	Ref<FRHIShaderProgram> Program = FindProgram(InProgramName);
	if (Program)
		return Program;

	FRHIGraphicsShaders Shaders;
	Shaders.VertexShader = FindShader(InVertexName, ERHIShaderStage::Vertex);
	Shaders.PixelShader = FindShader(InPixelName, ERHIShaderStage::Pixel);
	Shaders.HullShader = FindShader(InHullName, ERHIShaderStage::Hull);
	Shaders.DomainShader = FindShader(InDomainName, ERHIShaderStage::Domain);
	Shaders.GeometryShader = FindShader(InGeometryName, ERHIShaderStage::Geometry);

	switch (GetRenderAPI())
	{
	case ERenderAPI::OpenGL:
	{
		Program = FOpenGLShaderFactory::CreateShaderProgram(InProgramName, Shaders);
		if (Program)
		{
			mShaderProgramCache.emplace(InProgramName, Program);
		}
		return Program;
	}
	default:
		RE_CORE_ASSERT(false, "Unknown RenderAPI!");
		break;
	}
	return nullptr;
}

Ref<FRHIShaderProgram> FRHIShaderLibrary::FindProgram(const std::string& InName)
{
	if(InName.empty()) return nullptr;
	if (auto Iter = mShaderProgramCache.find(InName); Iter != mShaderProgramCache.end())
	{
		return Iter->second;
	}
	return nullptr;
}

void FRHIShaderLibrary::ClearShadersCache()
{
	mGraphicsShaderCache.clear();
}

void FRHIShaderLibrary::ClearShaderProgramsCache()
{
	mShaderProgramCache.clear();
}



}