#include "Rev/Render/Material/Material.h"
#include "Rev/Render/Material/MaterialUniform.h"
#include "Rev/Render/RenderCmd.h"

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
	RenderCmd::BindProgram(mProgram);
}

void Material::PostDraw()
{
	//RenderCmd::BindProgram(nullptr);
}

}


