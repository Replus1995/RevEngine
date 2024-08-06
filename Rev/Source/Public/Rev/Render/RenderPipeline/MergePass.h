#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderPipeline/FullScreenPass.h"

namespace Rev
{
class FRHIShaderProgram;
class FMergePass : public FFullScreenPass
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