#include "Rev/Render/Material/SurfaceMaterial.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"

namespace Rev
{

SurfaceMaterial::SurfaceMaterial()
{
	mDomain = MD_Surface;
}

SurfaceMaterial::~SurfaceMaterial()
{
}

void SurfaceMaterial::Compile()
{
	mProgram = FRHIShaderLibrary::GetInstance().CreateGraphicsProgram("BasicProgram",
		{ "/Engine/Shaders/BasicVS" },
		{ "/Engine/Shaders/BasicFS" });
}

void SurfaceMaterial::PreDraw(FRHICommandList& RHICmdList)
{
	//RenderCmd::SetBlendMode(BlendMode);
	//RenderCmd::SetCullFaceMode(TwoSided ? CFM_Disabled : CFM_Back);
	Material::PreDraw(RHICmdList);
}

void SurfaceMaterial::PostDraw(FRHICommandList& RHICmdList)
{
	Material::PostDraw(RHICmdList);
}

}