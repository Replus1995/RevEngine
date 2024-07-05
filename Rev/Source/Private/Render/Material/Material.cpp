#include "Rev/Render/Material/Material.h"

namespace Rev
{

std::string MaterialUniform::GetProgramName(const std::string& name)
{
	static constexpr std::string_view sProgramNamePrefix = "u_";
	return std::string(sProgramNamePrefix) + name;
}

Material::Material(const Ref<FRHIShader>& program)
	:mProgram(program)
{
	//Create Params
}

Material::~Material()
{
}

void Material::SetDomain(EMaterialDomain domain)
{
	mDomain = domain;
}


EMaterialDomain Material::GetDomain() const
{
	return mDomain;
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


