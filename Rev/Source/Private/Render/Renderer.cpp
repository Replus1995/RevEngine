#include "Rev/Render/Renderer.h"
#include "Rev/Render/RenderGraph/RenderGraphBuilder.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RHI/RHIPipeline.h"
#include "Rev/Render/RenderUtils.h"
#include "Rev/Render/UniformLayout.h"

namespace Rev
{
namespace RendererPrivate
{
struct FGBufferParameters { FRGTextureHandle A, B, C, Depth; };
struct FLightingParameters { FRGTextureHandle A, B, C, Depth, SceneColor; };
struct FSkyParameters { FRGTextureHandle SceneColor, Depth; };
struct FTonemapParameters { FRGTextureHandle SceneColor, BackBuffer; };
struct alignas(16) FDeferredDebugUniform { uint32 Mode = 0; uint32 Padding[3] = {}; };

FRHIGraphicsPipelineStateDesc MakePipeline(bool bMesh, ECompareFunction DepthCompare, bool bDepthWrite)
{
	FRHIGraphicsPipelineStateDesc State;
	State.VertexInputState = bMesh ? GStaticMeshVertexInputState.VertexInputStateRHI.get() : GTileVertexInputState.VertexInputStateRHI.get();
	FRHIRasterizerStateDesc Raster; Raster.CullMode = bMesh ? CM_Back : CM_None;
	State.RasterizerState = FRHIPipelineStateCache::Get()->GetOrCreateRasterizerState(Raster);
	FRHIDepthStencilStateDesc Depth; Depth.bEnableDepthWrite = bDepthWrite; Depth.DepthTestFunc = DepthCompare;
	State.DepthStencilState = FRHIPipelineStateCache::Get()->GetOrCreateDepthStencilState(Depth);
	FRHIColorBlendStateDesc Blend;
	for (uint32 I = 0; I < RTA_MaxColorAttachments; ++I) Blend.Attachments[I].bEnableBlend = false;
	State.ColorBlendState = FRHIPipelineStateCache::Get()->GetOrCreateColorBlendState(Blend);
	return State;
}
}
using namespace RendererPrivate;

FRenderer::FRenderer(FSceneProxy* InSceneProxy) : mSceneProxy(InSceneProxy) {}
FRenderer::~FRenderer() = default;

void FRenderer::InitializeDeferredPrograms()
{
	if (!DeferredLightingProgram)
		DeferredLightingProgram = FRHIShaderLibrary::Get()->CreateGraphicsProgram("DeferredLightingProgram", { "/Engine/Shaders/PostProcessVS" }, { "/Engine/Shaders/DeferredLightingPS" });
	if (!TonemapProgram)
		TonemapProgram = FRHIShaderLibrary::Get()->CreateGraphicsProgram("TonemapProgram", { "/Engine/Shaders/PostProcessVS" }, { "/Engine/Shaders/TonemapPS" });
	if (!DebugUniformBuffer) DebugUniformBuffer = GDynamicRHI->RHICreateUniformBuffer(sizeof(FDeferredDebugUniform));
}

void FRenderer::BeginFrame(FRHICommandList& Cmd)
{
	FrameWidth = Cmd.GetFrameWidth();
	FrameHeight = Cmd.GetFrameHeight();
}

void FRenderer::DrawFrame(FRHICommandList& Cmd)
{
	if (!FrameWidth || !FrameHeight) return;
	InitializeDeferredPrograms();
	mSceneProxy->SyncResource(Cmd);
	FDeferredDebugUniform Debug; Debug.Mode = uint32(DebugView); DebugUniformBuffer->UpdateSubData(&Debug, sizeof(Debug));
	Cmd.BindUniformBuffer(UL::BDeferredDebug, DebugUniformBuffer.get());

	FRGBuilder Graph(TransientPool, FRGName("DeferredRenderer"));
	const ETextureCreateFlags GBufferFlags = ETextureCreateFlags::ColorTarget | ETextureCreateFlags::ShaderResource;
	FRGTextureHandle A = Graph.CreateTexture(FRGTextureDesc::Create2D(FrameWidth, FrameHeight, PF_R8G8B8A8, Math::FLinearColor(0, 0, 0, 1), GBufferFlags), FRGName("GBufferA"));
	FRGTextureHandle B = Graph.CreateTexture(FRGTextureDesc::Create2D(FrameWidth, FrameHeight, PF_R16G16B16A16_SNORM, Math::FLinearColor(0, 0, 1, 1), GBufferFlags), FRGName("GBufferB"));
	FRGTextureHandle C = Graph.CreateTexture(FRGTextureDesc::Create2D(FrameWidth, FrameHeight, PF_R16G16B16A16_FLOAT, Math::FLinearColor(0, 0, 0, 0), GBufferFlags), FRGName("GBufferC"));
	FRGTextureHandle Depth = Graph.CreateTexture(FRGTextureDesc::Create2D(FrameWidth, FrameHeight, PF_DepthStencil, FRHITextureClearColor(0.0f, 0), ETextureCreateFlags::DepthStencilTarget | ETextureCreateFlags::ShaderResource), FRGName("SceneDepth"));
	FRGTextureHandle SceneColor = Graph.CreateTexture(FRGTextureDesc::Create2D(FrameWidth, FrameHeight, PF_R16G16B16A16_FLOAT, Math::FLinearColor(0, 0, 0, 1), GBufferFlags), FRGName("SceneColor"));
	FRHITexture* BackRaw = Cmd.GetBackTexture();
	FRHITextureRef Back(BackRaw, [](FRHITexture*) {});
	FRGTextureHandle BackBuffer = Graph.RegisterExternalTexture(Back, ERHIAccess::Present, ERHIAccess::Present, FRGName("BackBuffer"));

	const FGBufferParameters& GBuffer = Graph.AddPass<FGBufferParameters>(FRGName("GBuffer"), ERGPassFlags::Raster,
		[&](FRGPassBuilder& Builder, FGBufferParameters& P) { P.A = Builder.UseColorAttachment(0, A, RTL_Clear); P.B = Builder.UseColorAttachment(1, B, RTL_Clear); P.C = Builder.UseColorAttachment(2, C, RTL_Clear); P.Depth = Builder.UseDepthStencil(Depth, RTL_Clear, RTS_Store, false); },
		[this](FRHICommandList& RHICmd, const FGBufferParameters&) { RHICmd.SetGraphicsPipelineState(MakePipeline(true, CF_Greater, true)); mSceneProxy->DrawSceneOpaque(RHICmd); });

	const FLightingParameters& Lighting = Graph.AddPass<FLightingParameters>(FRGName("DeferredLighting"), ERGPassFlags::Raster,
		[&](FRGPassBuilder& Builder, FLightingParameters& P) { P.A = Builder.ReadTexture(GBuffer.A); P.B = Builder.ReadTexture(GBuffer.B); P.C = Builder.ReadTexture(GBuffer.C); P.Depth = Builder.ReadTexture(GBuffer.Depth); P.SceneColor = Builder.UseColorAttachment(0, SceneColor, RTL_Clear); },
		[this, &Graph](FRHICommandList& RHICmd, const FLightingParameters& P) {
			RHICmd.BindProgram(DeferredLightingProgram.get());
			RHICmd.BindTexture(UL::SGBufferA, Graph.GetTexture(P.A), GDefaultSamplerState.SamplerStateRHI.get());
			RHICmd.BindTexture(UL::SGBufferB, Graph.GetTexture(P.B), GDefaultSamplerState.SamplerStateRHI.get());
			RHICmd.BindTexture(UL::SGBufferC, Graph.GetTexture(P.C), GDefaultSamplerState.SamplerStateRHI.get());
			RHICmd.BindTexture(UL::SSceneDepth, Graph.GetTexture(P.Depth), GDefaultSamplerState.SamplerStateRHI.get());
			RHICmd.SetGraphicsPipelineState(MakePipeline(false, CF_Always, false));
			FRenderUtils::PostProcessDraw(RHICmd);
			RHICmd.BindProgram(nullptr);
		});

	const FSkyParameters& Sky = Graph.AddPass<FSkyParameters>(FRGName("Sky"), ERGPassFlags::Raster,
		[&](FRGPassBuilder& Builder, FSkyParameters& P) { P.SceneColor = Builder.UseColorAttachment(0, Lighting.SceneColor, RTL_Load); P.Depth = Builder.UseDepthStencil(GBuffer.Depth, RTL_Load, RTS_DontCare, true); },
		[this](FRHICommandList& RHICmd, const FSkyParameters&) { RHICmd.SetGraphicsPipelineState(MakePipeline(false, CF_GreaterEqual, false)); mSceneProxy->DrawSkybox(RHICmd); });

	const FTonemapParameters& Tonemap = Graph.AddPass<FTonemapParameters>(FRGName("Tonemap"), ERGPassFlags::Raster,
		[&](FRGPassBuilder& Builder, FTonemapParameters& P) { P.SceneColor = Builder.ReadTexture(Sky.SceneColor); P.BackBuffer = Builder.UseColorAttachment(0, BackBuffer, RTL_DontCare); },
		[this, &Graph](FRHICommandList& RHICmd, const FTonemapParameters& P) {
			RHICmd.BindProgram(TonemapProgram.get());
			RHICmd.BindTexture(UL::SSceneColor, Graph.GetTexture(P.SceneColor), GDefaultSamplerState.SamplerStateRHI.get());
			RHICmd.SetGraphicsPipelineState(MakePipeline(false, CF_Always, false));
			FRenderUtils::PostProcessDraw(RHICmd);
			RHICmd.BindProgram(nullptr);
		});
	Graph.Present(Tonemap.BackBuffer);
	Graph.Compile();
	Graph.Execute(Cmd);
}

void FRenderer::EndFrame(FRHICommandList&) {}
}
