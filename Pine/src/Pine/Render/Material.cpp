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

void Material::UploadUniform()
{
	uTransform.Upload(mProgram);
}

}


