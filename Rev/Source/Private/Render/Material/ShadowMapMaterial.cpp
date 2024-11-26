#include "Rev/Render/Material/ShadowMapMaterial.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"

namespace Rev
{

FShadowMapMaterial::FShadowMapMaterial()
{
}

FShadowMapMaterial::~FShadowMapMaterial()
{
}

void FShadowMapMaterial::Compile()
{
	ShaderProgram = FRHIShaderLibrary::Get()->CreateGraphicsProgram("PbrProgram",
		{ "/Engine/Shaders/ShadowMapVS" },
		{ "/Engine/Shaders/EmptyPS" }
	);
}

}