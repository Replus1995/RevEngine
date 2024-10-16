#include "Rev/Render/Material/Material.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHICommandList.h"

namespace Rev
{

Material::Material()
{
}

Material::~Material()
{
}

void Material::PreDraw(FRHICommandList& RHICmdList)
{
	RHICmdList.GetContext()->BindProgram(mProgram);
}

void Material::PostDraw(FRHICommandList& RHICmdList)
{
	RHICmdList.GetContext()->BindProgram(nullptr);
}

}


