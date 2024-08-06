#include "Rev/Render/RenderPipeline/MergePass.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"

namespace Rev 
{

FMergePass::FMergePass()
	: FFullScreenPass("MergePass")
{
}

FMergePass::~FMergePass()
{
}

void FMergePass::BeginPass()
{
	if (!mMergeProgram)
	{
		mMergeProgram = FRHIShaderLibrary::GetInstance().CreateGraphicsProgram("FMergeProgram", "/Engine/Shaders/PostVS", "/Engine/Shaders/PBR/MergePS");
	}
	FFullScreenPass::BeginPass();
	RenderCmd::BindProgram(mMergeProgram);
}

void FMergePass::EndPass()
{
	FFullScreenPass::EndPass();
}

}


