#include "ShadowRendering.h"
#include "Rev/Render/RenderProxy/SceneProxy.h"
#include "Rev/Render/RenderProxy/LightProxy.h"
#include "Rev/Render/RenderProxy/CameraProxy.h"
#include "Rev/Render/RenderUtils.h"
#include "Rev/Render/RenderOptions.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Render/RHI/RHIPipeline.h"
#include "Rev/Render/UniformDefine.h"
#include "Rev/Render/UniformLayout.h"
#include "Rev/Math/Maths.h"

namespace Rev
{
    using namespace Math;

static constexpr uint32 kShadowMapSize = 2048;
static constexpr uint32 kNumCascades = 4;
static constexpr float kCascadeLambda = 0.5f;

FFGShadowPass::FFGShadowPass(FFrameGraph& InGraph, const FFGViewData& InViewData)
{
    // Shadow map texture descriptor (2D array, one layer per cascade)
    FRHITextureDesc ShadowDesc = FRHITextureDesc::Create2DArray(
        kShadowMapSize, kShadowMapSize, kNumCascades, PF_ShadowDepth
    ).SetClearColor(FRHITextureClearColor(1.0f, 0))
     .SetFlags(ETextureCreateFlags::DepthStencilTarget | ETextureCreateFlags::ShaderResource);

    ShadowDesc.NumSamples = 1;

    InGraph.AddPass<FFGShadowPass::Data>(
        "Shadow Pass",
        [&](FFGBuilder& InBuilder, FFGShadowPass::Data& InData) {
            FFGHandle CreatedShadowMap = InBuilder.Create<FFGTexture>("ShadowMap", ShadowDesc);
            InData.ShadowMap = InBuilder.Write(CreatedShadowMap);
            InBuilder.SetSideEffect();

            InData.SetDepthStencilTarget(InData.ShadowMap, KFGInvalidHandle, RTL_Clear, RTL_Clear);
        },
        [=](const FFGShadowPass::Data& InData, FFGPassResources& InResources, FFGContextData& InContextData) {

            auto& RHICmdList = InContextData.RHICmdList;
            FSceneProxy* SceneProxy = InContextData.SceneProxy;

            // Get shadow map texture
            auto& ShadowTex = InResources.Get<FFGTexture>(InData.ShadowMap);

            // Get camera data
            const FMatrix4& ProjMat = SceneProxy->mCameraProxy.GetProjMat();
            const FMatrix4& ViewMat = SceneProxy->mCameraProxy.GetViewMat();
            FMatrix4 ViewProjMat = ProjMat * ViewMat;
            FMatrix4 InvViewProj = ViewProjMat.Inverse();

            // Get light direction
            const FDirectionalLightUniform& LightParams = SceneProxy->mLightProxy.GetDirectionalLightParams();
            FVector3 LightDir;
            if (LightParams.Count > 0)
            {
                LightDir = FVector3(
                    -LightParams.Lights[0].Direction.X,
                    -LightParams.Lights[0].Direction.Y,
                    -LightParams.Lights[0].Direction.Z
                );
                LightDir.Normalize();
            }
            else
            {
                LightDir = FVector3(0.3f, -1.0f, 0.5f).Normalized();
            }

            // Camera near/far
            float CameraNear = 0.01f;
            float CameraFar = 1000.0f;

            // Calculate cascade splits
            auto CascadeSplits = Math::CalculateCascadeSplits(CameraNear, CameraFar, kNumCascades, kCascadeLambda);

            // Create shadow depth shader program
            Ref<FRHIShaderProgram> ShadowProgram = FRHIShaderLibrary::Get()->CreateGraphicsProgram("/Engine/Shaders/ShadowDepth", 
                FRHIShaderCreateDesc("/Engine/Shaders/ShadowDepthVS"),
                FRHIShaderCreateDesc("/Engine/Shaders/ShadowDepthPS")
            );

            if (!ShadowProgram)
                return;

            // Render each cascade
            for (uint32 CascadeIndex = 0; CascadeIndex < kNumCascades; ++CascadeIndex)
            {
                float SplitNear = CascadeSplits[CascadeIndex] / CameraFar;
                float SplitFar = CascadeSplits[CascadeIndex + 1] / CameraFar;

                auto CascadeCorners = Math::GetFrustumCorners(InvViewProj, SplitNear, SplitFar);

                FMatrix4 LightViewProj;
                Math::CalculateLightViewProj(CascadeCorners, LightDir, LightViewProj);

                // Depth-only render pass for this cascade layer
                FRHIRenderPassDesc RenderPassDesc;
                RenderPassDesc.DepthStencilRenderTarget.DepthStencilTarget = ShadowTex.GetTextureRHI();
                RenderPassDesc.DepthStencilRenderTarget.ArraySlice = (int32)CascadeIndex;
                RenderPassDesc.DepthStencilRenderTarget.MipIndex = 0;
                RenderPassDesc.DepthStencilRenderTarget.DepthLoadAction = RTL_Clear;
                RenderPassDesc.DepthStencilRenderTarget.DepthStoreAction = RTS_Store;
                RenderPassDesc.DepthStencilRenderTarget.StencilLoadAction = RTL_DontCare;
                RenderPassDesc.DepthStencilRenderTarget.StencilStoreAction = RTS_DontCare;

                auto ShadowRenderPass = GDynamicRHI->RHICreateRenderPass(RenderPassDesc);
                RHICmdList.GetContext()->RHIBeginRenderPass(ShadowRenderPass.get());

                // Bind shadow shader
                RHICmdList.GetContext()->RHIBindProgram(ShadowProgram.get());

                // Upload per-cascade LightViewProj to BShadow
                struct { FMatrix4 LightViewProj; } CascadeUB;
                CascadeUB.LightViewProj = LightViewProj;

                Ref<FRHIUniformBuffer> CascadeUniformBuffer = GDynamicRHI->RHICreateUniformBuffer(sizeof(CascadeUB));
                CascadeUniformBuffer->UpdateSubData(&CascadeUB, sizeof(CascadeUB));
                RHICmdList.GetContext()->RHIBindUniformBuffer(UL::BCascadeShadow, CascadeUniformBuffer.get());

                FRHIGraphicsPipelineStateDesc PipelineStateDesc;
                PipelineStateDesc.VertexInputState = GStaticMeshVertexInputState.VertexInputStateRHI.get();
                PipelineStateDesc.NumSamples = GRenderOptions.GetNumSamples();

                FRHIRasterizerStateDesc RasterizerStateDesc;
                RasterizerStateDesc.CullMode = CM_Back;
                PipelineStateDesc.RasterizerState = FRHIPipelineStateCache::Get()->GetOrCreateRasterizerState(RasterizerStateDesc);

                FRHIDepthStencilStateDesc DepthStencilStateDesc;
                DepthStencilStateDesc.bEnableDepthWrite = true;
                DepthStencilStateDesc.DepthTestFunc = CF_Less;
                PipelineStateDesc.DepthStencilState = FRHIPipelineStateCache::Get()->GetOrCreateDepthStencilState(DepthStencilStateDesc);

                FRHIColorBlendStateDesc ColorBlendStateDesc;
                ColorBlendStateDesc.Attachments[0].bEnableBlend = false;
                PipelineStateDesc.ColorBlendState = FRHIPipelineStateCache::Get()->GetOrCreateColorBlendState(ColorBlendStateDesc);

                RHICmdList.GetContext()->RHISetGraphicsPipelineState(PipelineStateDesc);

                // Draw depth-only
                SceneProxy->mStaticMeshProxy.DrawMeshesDepth(RHICmdList);

                RHICmdList.GetContext()->RHIEndRenderPass();
            }

            // Build full shadow uniform for PBR pass (all cascades)
            FShadowUniform FullShadowUniform;
            for (uint32 i = 0; i < kNumCascades; ++i)
            {
                float SplitNear = CascadeSplits[i] / CameraFar;
                float SplitFar = CascadeSplits[i + 1] / CameraFar;
                auto CascadeCornersLocal = Math::GetFrustumCorners(InvViewProj, SplitNear, SplitFar);
                FMatrix4 CascadeLightViewProj;
                Math::CalculateLightViewProj(CascadeCornersLocal, LightDir, CascadeLightViewProj);
                FullShadowUniform.LightViewMats[i] = CascadeLightViewProj;
            }
            FullShadowUniform.LightParams = FVector4(
                (float)kNumCascades,
                CascadeSplits[1],
                CascadeSplits[2],
                CascadeSplits[3]
            );

            Ref<FRHIUniformBuffer> ShadowUB = GDynamicRHI->RHICreateUniformBuffer(sizeof(FShadowUniform));
            ShadowUB->UpdateSubData(&FullShadowUniform, sizeof(FShadowUniform));
            RHICmdList.GetContext()->RHIBindUniformBuffer(UL::BShadow, ShadowUB.get());

            // Bind shadow texture with comparison sampler
            FRHISamplerStateDesc ShadowSamplerDesc;
            ShadowSamplerDesc.Filter = SF_Bilinear;
            ShadowSamplerDesc.WarpU = SW_Clamp;
            ShadowSamplerDesc.WarpV = SW_Clamp;
            ShadowSamplerDesc.WarpW = SW_Clamp;
            ShadowSamplerDesc.CompareFunc = SCF_Less;

            Ref<FRHISamplerState> ShadowSampler = GDynamicRHI->RHICreateSamplerState(ShadowSamplerDesc);
            RHICmdList.GetContext()->RHIBindTexture(UL::SShadowMap, ShadowTex.GetTextureRHI(), ShadowSampler.get());
        }
    );
}

}
