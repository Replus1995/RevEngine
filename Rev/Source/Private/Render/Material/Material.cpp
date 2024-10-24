#include "Rev/Render/Material/Material.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHICommandList.h"

namespace Rev
{

FMaterial::FMaterial()
{
}

FMaterial::~FMaterial()
{
}

void FMaterial::PreDraw(FRHICommandList& RHICmdList)
{
	RHICmdList.GetContext()->BindProgram(ShaderProgram.get());
}

void FMaterial::PostDraw(FRHICommandList& RHICmdList)
{
	RHICmdList.GetContext()->BindProgram(nullptr);
}

}


