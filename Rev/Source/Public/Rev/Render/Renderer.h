#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderProxy/SceneProxy.h"
#include "Rev/Render/RenderGraph/RenderGraphResources.h"

namespace Rev
{
class FRHIShaderProgram;
class FRHIUniformBuffer;
class FCascadeShadowMap;
enum class EDeferredDebugView : uint32 { Final, BaseColor, Normal, Roughness, Metallic, AO, Emissive, Depth, ShadowCascades };
class FRenderer
{
public:
	explicit FRenderer(FSceneProxy* InSceneProxy);
	virtual ~FRenderer();
	virtual void BeginFrame(FRHICommandList&);
	virtual void DrawFrame(FRHICommandList&);
	virtual void EndFrame(FRHICommandList&);
	void SetDebugView(EDeferredDebugView InView) { DebugView = InView; }
private:
	void InitializeDeferredPrograms();
	FSceneProxy* mSceneProxy = nullptr;
	uint32 FrameWidth = 0;
	uint32 FrameHeight = 0;
	FRGTransientResourcePool TransientPool;
	Ref<FRHIShaderProgram> DeferredLightingProgram;
	Ref<FRHIShaderProgram> TonemapProgram;
	Ref<FRHIUniformBuffer> DebugUniformBuffer;
	Scope<FCascadeShadowMap> CascadeShadowMap;
	EDeferredDebugView DebugView = EDeferredDebugView::Final;
};
}
