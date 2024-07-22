#include "Rev/Render/RenderPipeline/MergePass.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"

namespace Rev 
{

FMergePass::FMergePass()
	: FPostPass("MergePass")
{
}

FMergePass::~FMergePass()
{
}

void FMergePass::BeginPass()
{
	if (!mMergeProgram)
	{
		mMergeProgram = FRHIShaderLibrary::GetInstance().CreateGraphicsProgram("FMergeProgram", "/Engine/Shaders/PBR/MergeVS", "/Engine/Shaders/PBR/MergePS");
	}
	FPostPass::BeginPass();
	mMergeProgram->Bind();
}

void FMergePass::EndPass()
{
	mMergeProgram->Unbind();
	FPostPass::EndPass();
}

}


