#include "pinepch.h"
#include "Material.h"

namespace Pine
{

Material::Material(const Ref<Shader>& program)
	:mProgram(program)
{
	//Create Params
}

Material::~Material()
{
}

void Material::Bind()
{
	mProgram->Bind();
	UploadUniform();
}

void Material::Unbind()
{
	mProgram->Unbind();
}

MaterialUniform* Material::FindUIniform(std::string_view name)
{
	if (auto iter = mUniformMap.find(name); iter != mUniformMap.end())
	{
		return iter->second.get();
	}
	return nullptr;
}

void Material::UploadUniform()
{
	for (auto iter = mUniformMap.begin(); iter != mUniformMap.end(); iter++)
	{
		iter->second->Upload(mProgram.get());
	}
}

}


