#include "Rev/Render/Material/Material.h"
#include "Rev/Render/RenderCmd.h"

namespace Rev
{

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
	RenderCmd::BindProgram(nullptr);
}

}


