#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIRenderTarget.h"
#include "Rev/Render/RenderPipeline/RenderPass.h"

namespace Rev
{

class FScreenPass : FRHIRenderPass
{
public:
	FScreenPass(std::string_view InName, bool bOffScreen);
	virtual ~FScreenPass();
	virtual void BeginPass() override;
	virtual void EndPass() override;

protected:
	virtual FRenderTargetDesc MakeTargetDesc(uint32 InWidth, uint32 InHeight) = 0;
	virtual void PrepareTarget();

protected:
	bool mOffScreen;

};


}
