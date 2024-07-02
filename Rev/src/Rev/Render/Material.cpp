#include "pinepch.h"
#include "Material.h"

namespace Pine
{

std::string MaterialUniform::GetProgramName(const std::string& name)
{
	static constexpr std::string_view sProgramNamePrefix = "u_";
	return std::string(sProgramNamePrefix) + name;
}

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

