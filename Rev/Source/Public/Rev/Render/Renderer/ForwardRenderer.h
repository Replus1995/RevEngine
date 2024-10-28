#pragma once
#include "Rev/Render/Renderer/Renderer.h"
#include "Rev/Render/RenderPass/GammaPass.h"
#include "Rev/Render/RHI/RHIRenderPass.h"

namespace Rev
{
class FRHITexture;
class FRHIRenderPass;
class FForwardRenderer : public FRenderer
{
public:
	FForwardRenderer(FSceneProxy* InSceneProxy);
	virtual ~FForwardRenderer();

	virtual void BeginFrame() override;
	virtual void DrawFrame(FRHICommandList& RHICmdList) override;
	virtual void EndFrame() override;

private:
	uint32 FrameWidth = 0;
	uint32 FrameHeight = 0;
	Ref<FRHIRenderPass> mBasePass = nullptr;
	Ref<FRHITexture> mBasePassColor = nullptr;
	Ref<FRHITexture> mBasePassDepth = nullptr;

	FGammaCorrectPass mGammaCorrectPass;
};

}