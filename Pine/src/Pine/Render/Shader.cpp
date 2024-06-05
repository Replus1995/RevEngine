#include "pinepch.h"
#include "Pine/Render/Shader.h"
#include "Pine/Core/Base.h"

namespace Pine
{

std::shared_ptr<Shader> Shader::Create(const std::string& filepath)
{
	return std::shared_ptr<Shader>();
}

std::shared_ptr<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
{
	return std::shared_ptr<Shader>();
}

void ShaderLibrary::Add(const std::string& name, const std::shared_ptr<Shader>& shader)
{
	PE_CORE_ASSERT(!Exists(name), "[ShaderLibrary] Shader already exists!");
	mShaderCache[name] = shader;
}

void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader)
{
	auto& name = shader->GetName();
	Add(name, shader);
}

std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& filepath)
{
	auto shader = Shader::Create(filepath);
	Add(shader);
	return shader;
}

std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
{
	auto shader = Shader::Create(filepath);
	Add(name, shader);
	return shader;
}

std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& name)
{
	PE_CORE_ASSERT(Exists(name), "Shader not found!");
	return mShaderCache[name];
}

bool ShaderLibrary::Exists(const std::string& name) const
{
	return mShaderCache.find(name) != mShaderCache.end();
}

}