#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIRenderPass.h"

namespace Rev
{

class FScreenPass : FRHIRenderPass
{
public:
	FScreenPass(std::string_view InName, bool bOffScreen);
	virtual ~FScreenPass();
	virtual void BeginPass(uint32 InWidth, uint32 InHeight) override;
	virtual void EndPass() override;

protected:
	virtual FRenderTargetDesc MakeTargetDesc(uint32 InWidth, uint32 InHeight) = 0;
	virtual void PrepareTarget(uint32 InWidth, uint32 InHeight);

protected:
	bool mOffScreen;

};


}
