#include "Rev/Render/RHI/RHIShader.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Core/Assert.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Rev
{

Ref<FRHIShader> FRHIShader::Create(const std::string& filepath)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<OpenGLShader>(filepath);
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHIShader> FRHIShader::Create(const std::string& name, const std::string& vertSrc, const std::string& fragSrc)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<OpenGLShader>(name, vertSrc, fragSrc);
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

void FRHIShaderLibrary::Add(const std::string& name, const Ref<FRHIShader>& shader)
{
	RE_CORE_ASSERT(!Exists(name), "[ShaderLibrary] Shader already exists!");
	mShaderCache[name] = shader;
}

void FRHIShaderLibrary::Add(const Ref<FRHIShader>& shader)
{
	auto& name = shader->GetName();
	Add(name, shader);
}

Ref<FRHIShader> FRHIShaderLibrary::Load(const std::string& filepath)
{
	auto shader = FRHIShader::Create(filepath);
	Add(shader);
	return shader;
}

Ref<FRHIShader> FRHIShaderLibrary::Load(const std::string& name, const std::string& filepath)
{
	auto shader = FRHIShader::Create(filepath);
	Add(name, shader);
	return shader;
}

Ref<FRHIShader> FRHIShaderLibrary::Get(const std::string& name)
{
	RE_CORE_ASSERT(Exists(name), "Shader not found!");
	return mShaderCache[name];
}

bool FRHIShaderLibrary::Exists(const std::string& name) const
{
	return mShaderCache.find(name) != mShaderCache.end();
}

}