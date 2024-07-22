#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderPipeline/PostPass.h"

namespace Rev
{
class FRHIShaderProgram;
class FMergePass : public FPostPass
{
public:
	FMergePass();
	virtual ~FMergePass();
	virtual void BeginPass() override;
	virtual void EndPass() override;

private:
	Ref<FRHIShaderProgram> mMergeProgram;
};

}