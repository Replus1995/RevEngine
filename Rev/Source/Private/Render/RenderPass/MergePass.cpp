#include "Rev/Render/RenderPass/MergePass.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"

namespace Rev 
{

FMergePass::FMergePass()
	: FRenderPass("MergePass")
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
	FRenderPass::BeginPass();
	RenderCmd::BindProgram(mMergeProgram);
}

void FMergePass::EndPass()
{
	FRenderPass::EndPass();
}

}


