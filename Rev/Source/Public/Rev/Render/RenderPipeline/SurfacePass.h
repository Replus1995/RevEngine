#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderPipeline/ScreenPass.h"

namespace Rev
{

class FSurfacePass : public FScreenPass
{
public:
	FSurfacePass();
	virtual ~FSurfacePass();

	virtual void BeginPass() override;
	virtual void EndPass() override;

protected:
	virtual FRenderTargetDesc MakeTargetDesc(uint32 InWidth, uint32 InHeight) override;
};

}