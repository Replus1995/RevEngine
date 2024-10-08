#pragma once
#include "Rev/Render/Renderer/Renderer.h"
#include "Rev/Render/RenderPass/SurfacePass.h"
#include "Rev/Render/RenderPass/GammaPass.h"

namespace Rev
{

class FForwardRenderer : public FRenderer
{
public:
	FForwardRenderer(FSceneProxy* InSceneProxy);
	virtual ~FForwardRenderer();

	virtual void BeginFrame() override;
	virtual void DrawFrame() override;
	virtual void EndFrame() override;

private:
	Ref<class FRHIRenderTarget> mLinearScreenTarget = nullptr;
	FForwardSurfacePass mForwardSurfacePass;
	FGammaCorrectPass mGammaCorrectPass;
};

}