#include "Rev/Render/Material/Material.h"
#include "Rev/Render/Material/MaterialUniform.h"
#include "Rev/Render/Texture/Texture.h"
#include "Rev/Render/RHI/RHITexture.h"
#include <Rev/Render/RHI/RHIShaderLibrary.h>

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

void Material::Compile()
{
	mProgram = FRHIShaderLibrary::GetInstance().CreateGraphicsProgram("BasicProgram",
		{ "/Engine/Shaders/BasicVS" },
		{ "/Engine/Shaders/BasicFS" });
}

void Material::PreDraw()
{
}

void Material::PostDraw()
{
}


}


