#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderPipeline/RenderPipeline.h"
#include "Rev/Render/RenderPipeline/SurfacePass.h"
#include "Rev/Render/RenderPipeline/GammaPass.h"
#include "Rev/Render/RHI/RHIRenderTarget.h"

namespace Rev
{

class FForwardPipeline : public FRenderPipeline
{
public:
	FForwardPipeline();
	virtual ~FForwardPipeline();

	virtual void BeginPipeline(uint32 InWidth, uint32 InHeight);
	virtual void RunPipeline(SceneRenderProxy& InSceneProxy);
	virtual void EndPipeline();

protected:
	Ref<FRHIRenderTarget> mLinearScreenTarget = nullptr;
	FForwardSurfacePass mForwardSurfacePass;
	FGammaCorrectPass mGammaCorrectPass;
};

}