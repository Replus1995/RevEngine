#include "Rev/Render/RenderPipeline/GammaPass.h"
#include "Rev/Render/RHI/RHIResourceFactory.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"


namespace Rev
{

FGammaCorrectPass::FGammaCorrectPass()
	: FPostPass("GammaCorrectPass")
{
	mProgram = FRHIShaderLibrary::GetInstance().CreateGraphicsProgram(
		"GammaCorrectProgram",
		{ "/Engine/Shaders/PostVS" },
		{ "/Engine/Shaders/GammaCorrectFS" }
	);
}

FGammaCorrectPass::~FGammaCorrectPass()
{
}


}

