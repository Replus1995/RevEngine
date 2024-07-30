#include "Rev/Render/Material/Material.h"
#include "Rev/Render/Material/MaterialUniform.h"

namespace Rev
{

std::string MaterialUniform::GetProgramName(const std::string& name)
{
	static constexpr std::string_view sProgramNamePrefix = "u_";
	return std::string(sProgramNamePrefix) + name;
}

Material::Material()
{
}

Material::~Material()
{
}

void Material::PreDraw()
{
	if(mProgram)
		mProgram->Bind();
}

void Material::PostDraw()
{
	if (mProgram)
		mProgram->Unbind();
}

}


