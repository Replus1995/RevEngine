#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderPass/RenderPass.h"

namespace Rev
{
class FRHIShaderProgram;
class FMergePass : public FRenderPass
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