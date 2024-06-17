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
	for (auto iter = mParamMap.begin(); iter != mParamMap.end(); iter++)
	{
		iter->second->Upload(mProgram.get());
	}
}

void Material::Unbind()
{
	mProgram->Unbind();
}

MaterialUniform* Material::FindParam(std::string_view name)
{
	if (auto iter = mParamMap.find(name); iter != mParamMap.end())
	{
		return iter->second.get();
	}
	return nullptr;
}

}


