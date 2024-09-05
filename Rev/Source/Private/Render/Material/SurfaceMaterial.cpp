#include "Rev/Render/Material/SurfaceMaterial.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Render/RenderCmd.h"

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

void SurfaceMaterial::PreDraw()
{
	Material::PreDraw();
}

void SurfaceMaterial::PostDraw()
{
	Material::PostDraw();
}

}