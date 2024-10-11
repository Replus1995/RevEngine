#pragma once
#include "Rev/Render/Renderer/Renderer.h"
#include "Rev/Render/RenderPass/GammaPass.h"
#include "Rev/Render/RHI/RHIRenderPass.h"

namespace Rev
{
class FRHIRenderTarget;
class FRHIRenderPass;
class FForwardRenderer : public FRenderer
{
public:
	FForwardRenderer(FSceneProxy* InSceneProxy);
	virtual ~FForwardRenderer();

	virtual void BeginFrame() override;
	virtual void DrawFrame() override;
	virtual void EndFrame() override;

private:
	Ref<FRHIRenderPass> mBasePass = nullptr;
	Ref<FRHIRenderTarget> mBasePassTarget = nullptr;

	FGammaCorrectPass mGammaCorrectPass;
};

}