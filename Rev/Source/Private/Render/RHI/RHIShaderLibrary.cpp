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
static constexpr std::string_view sRHIShaderExtension = ".glsl";

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

FRHIGraphicsShaders FRHIShaderLibrary::LoadOrCompileShader(const FPath& InPath)
{
	auto CompiledData = FShadercFactory::LoadAndCompile(InPath);
	if (CompiledData.Empty())
	{
		RE_CORE_WARN("No shader complied for {0}", InPath.FullPath().c_str());
		return {};
	}

	switch (GetRenderAPI())
	{
	case ERenderAPI::OpenGL:
	{
		FRHIGraphicsShaders GraphicsShaders = FOpenGLShaderFactory::CreateGraphicsShaders(CompiledData);
		mGraphicsShaderCache.emplace(CompiledData.Name, GraphicsShaders);
		return GraphicsShaders;
	}
	default:
		RE_CORE_ASSERT(false, "Unknown RenderAPI!");
		break;
	}
	return {};
}

Ref<FRHIShader> FRHIShaderLibrary::FindShader(const std::string& InName, ERHIShaderStage InStage)
{
	if (InName.empty()) return nullptr;
	if (uint8(InStage) < uint8(ERHIShaderStage::Compute))
	{
		auto ShadersIter = mGraphicsShaderCache.find(InName);
		if (ShadersIter != mGraphicsShaderCache.end())
		{
			return ShadersIter->second[InStage];
		}
		else
		{
			return LoadOrCompileShader(InName + std::string(sRHIShaderExtension))[InStage];
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

	FRHIGraphicsShaders Shaders;
	auto ShadersIter = mGraphicsShaderCache.find(InShadersName);
	if (ShadersIter != mGraphicsShaderCache.end())
	{
		Shaders = ShadersIter->second;
	}
	else
	{
		Shaders = LoadOrCompileShader(InShadersName + std::string(sRHIShaderExtension));
	}
	
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