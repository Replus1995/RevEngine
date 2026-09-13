#include "CascadeShadowMap.h"
#include "Rev/Render/RenderOptions.h"
#include "Rev/Render/RenderGraph/RenderGraphBuilder.h"
#include "Rev/Render/RenderProxy/SceneProxy.h"
#include "Rev/Render/RenderProxy/CameraProxy.h"
#include "Rev/Render/RenderProxy/LightProxy.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RHI/RHIPipeline.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Render/RenderUtils.h"
#include "Rev/Render/UniformLayout.h"
#include <algorithm>
#include <array>
#include <cfloat>
#include <cmath>

namespace Rev
{
namespace CascadeShadowMapPrivate
{
Math::FVector3 TransformPoint(const Math::FMatrix4& Matrix, const Math::FVector3& Point)
{
	const Math::FVector4 Result(
		Matrix[0][0] * Point.X + Matrix[1][0] * Point.Y + Matrix[2][0] * Point.Z + Matrix[3][0],
		Matrix[0][1] * Point.X + Matrix[1][1] * Point.Y + Matrix[2][1] * Point.Z + Matrix[3][1],
		Matrix[0][2] * Point.X + Matrix[1][2] * Point.Y + Matrix[2][2] * Point.Z + Matrix[3][2],
		Matrix[0][3] * Point.X + Matrix[1][3] * Point.Y + Matrix[2][3] * Point.Z + Matrix[3][3]);
	return Math::FVector3(Result.X, Result.Y, Result.Z) / std::max(std::abs(Result.W), 0.000001f);
}

Math::FMatrix4 MakeLightView(const Math::FVector3& Center, const Math::FVector3& LightDirection, float Distance)
{
	const Math::FVector3 Forward = LightDirection.Normalized();
	const Math::FVector3 ReferenceUp = std::abs(Forward.Y) > 0.95f ? Math::FVector3(1.0f, 0.0f, 0.0f) : Math::FVector3(0.0f, 1.0f, 0.0f);
	const Math::FVector3 Right = Math::FVector3::Cross(Forward, ReferenceUp).Normalized();
	const Math::FVector3 Up = Math::FVector3::Cross(Right, Forward).Normalized();
	const Math::FVector3 Position = Center - Forward * Distance;
	return Math::FMatrix4(Math::FVector4(Right.X, Up.X, -Forward.X, 0.0f), Math::FVector4(Right.Y, Up.Y, -Forward.Y, 0.0f), Math::FVector4(Right.Z, Up.Z, -Forward.Z, 0.0f), Math::FVector4(-Math::FVector3::Dot(Right, Position), -Math::FVector3::Dot(Up, Position), Math::FVector3::Dot(Forward, Position), 1.0f));
}

std::array<Math::FVector3, 8> BuildFrustumCorners(const FCameraProxy& InCamera, float NearDistance, float FarDistance)
{
	const Camera::FCameraProjectionInfo Info = InCamera.GetProjectionInfo();
	std::array<Math::FVector3, 8> Result;
	const Math::FMatrix4 InverseView = InCamera.GetViewMat().Inverse();
	for (uint32 Plane = 0; Plane < 2; ++Plane)
	{
		const float Distance = Plane == 0 ? NearDistance : FarDistance;
		const float HalfY = Info.Type == Camera::ProjectionType::Perspective ? std::tan(Math::Radians(Info.VerticalFOV) * 0.5f) * Distance : Info.OrthographicSize * 0.5f;
		const float HalfX = HalfY * Info.AspectRatio;
		for (uint32 Corner = 0; Corner < 4; ++Corner)
			Result[Plane * 4 + Corner] = TransformPoint(InverseView, Math::FVector3((Corner & 1) ? HalfX : -HalfX, (Corner & 2) ? HalfY : -HalfY, -Distance));
	}
	return Result;
}

FRHIGraphicsPipelineStateDesc MakeShadowPipeline(float DepthBias, float SlopeBias)
{
	FRHIGraphicsPipelineStateDesc State;
	State.VertexInputState = GStaticMeshVertexInputState.VertexInputStateRHI.get();
	FRHIRasterizerStateDesc Raster; Raster.CullMode = CM_Back; Raster.DepthBias = DepthBias; Raster.DepthBiasSlopeFactor = SlopeBias;
	State.RasterizerState = FRHIPipelineStateCache::Get()->GetOrCreateRasterizerState(Raster);
	FRHIDepthStencilStateDesc Depth; Depth.bEnableDepthWrite = true; Depth.DepthTestFunc = CF_Greater;
	State.DepthStencilState = FRHIPipelineStateCache::Get()->GetOrCreateDepthStencilState(Depth);
	FRHIColorBlendStateDesc Blend; State.ColorBlendState = FRHIPipelineStateCache::Get()->GetOrCreateColorBlendState(Blend);
	return State;
}
}
using namespace CascadeShadowMapPrivate;

void FCascadeShadowMap::InitializeResources()
{
	if (!ShadowProgram) ShadowProgram = FRHIShaderLibrary::Get()->CreateGraphicsProgram("CascadeShadowProgram", { "/Engine/Shaders/CascadeShadowVS" }, {});
	if (!LightingUniformBuffer) LightingUniformBuffer = GDynamicRHI->RHICreateUniformBuffer(sizeof(FDirectionalShadowUniform));
	for (Ref<FRHIUniformBuffer>& Buffer : PassUniformBuffers) if (!Buffer) Buffer = GDynamicRHI->RHICreateUniformBuffer(sizeof(FShadowPassUniform));
	if (!ComparisonSampler)
	{
		FRHISamplerStateDesc Desc(SF_Bilinear, SW_Border, SW_Border, SW_Border); Desc.BorderColor = 1; Desc.CompareFunc = CF_GreaterEqual;
		ComparisonSampler = GDynamicRHI->RHICreateSamplerState(Desc);
	}
}

void FCascadeShadowMap::Prepare(const FCameraProxy& InCamera, FLightProxy& Lights)
{
	InitializeResources();
	const FCSMSettings& Settings = GRenderOptions.GetCSMSettings();
	Resolution = Settings.Resolution; bActive = Lights.HasShadowCastingDirectionalLight(); CascadeCount = bActive ? Settings.CascadeCount : 1;
	ShadowUniform = {}; for (Math::FMatrix4& Matrix : ShadowUniform.LightViewProjMats) Matrix = Math::FMatrix4(1.0f);
	if (!bActive)
	{
		ShadowUniform.Params = Math::FVector4(0.0f, 1.0f / float(Resolution), Settings.TransitionFraction, Settings.NormalBias);
		LightingUniformBuffer->UpdateSubData(&ShadowUniform, sizeof(ShadowUniform)); Lights.SetShadowCascadeCount(0); return;
	}
	const Camera::FCameraProjectionInfo Info = InCamera.GetProjectionInfo();
	const float NearClip = std::max(Info.NearClip, 0.01f);
	const float FarClip = std::max(NearClip + 0.01f, std::min(Info.FarClip, Settings.MaxDistance));
	std::array<float, REV_MAX_CSM_CASCADES + 1> Splits{}; Splits[0] = NearClip;
	for (uint32 Index = 1; Index <= CascadeCount; ++Index)
	{
		const float Fraction = float(Index) / float(CascadeCount);
		Splits[Index] = Settings.SplitLambda * NearClip * std::pow(FarClip / NearClip, Fraction) + (1.0f - Settings.SplitLambda) * (NearClip + (FarClip - NearClip) * Fraction);
	}
	for (uint32 Cascade = 0; Cascade < CascadeCount; ++Cascade)
	{
		const auto Corners = BuildFrustumCorners(InCamera, Splits[Cascade], Splits[Cascade + 1]);
		Math::FVector3 Center(0.0f); for (const Math::FVector3& Corner : Corners) Center += Corner; Center /= float(Corners.size());
		float Radius = 0.0f; for (const Math::FVector3& Corner : Corners) Radius = std::max(Radius, (Corner - Center).Length()); Radius = std::ceil(Radius * 16.0f) / 16.0f;
		Math::FMatrix4 View = MakeLightView(Center, Lights.GetShadowDirection(), Radius + Settings.CasterExtrusion);
		const float TexelSize = (Radius * 2.0f) / float(Resolution);
		View[3][0] = std::round(View[3][0] / TexelSize) * TexelSize;
		View[3][1] = std::round(View[3][1] / TexelSize) * TexelSize;
		float MinimumZ = FLT_MAX, MaximumZ = -FLT_MAX;
		for (const Math::FVector3& Corner : Corners) { const float Z = TransformPoint(View, Corner).Z; MinimumZ = std::min(MinimumZ, Z); MaximumZ = std::max(MaximumZ, Z); }
		const float LightNear = std::max(0.01f, -MaximumZ - Settings.CasterExtrusion); const float LightFar = std::max(LightNear + 0.01f, -MinimumZ + Settings.CasterExtrusion);
		ShadowUniform.LightViewProjMats[Cascade] = Math::FMatrix4::Othographic(-Radius, Radius, -Radius, Radius, LightNear, LightFar) * View;
		ShadowUniform.CascadeSplits[Cascade] = Splits[Cascade + 1]; FShadowPassUniform PassUniform{ ShadowUniform.LightViewProjMats[Cascade] }; PassUniformBuffers[Cascade]->UpdateSubData(&PassUniform, sizeof(PassUniform));
	}
	ShadowUniform.Params = Math::FVector4(float(CascadeCount), 1.0f / float(Resolution), Settings.TransitionFraction, Settings.NormalBias);
	LightingUniformBuffer->UpdateSubData(&ShadowUniform, sizeof(ShadowUniform)); Lights.SetShadowCascadeCount(CascadeCount);
}

FRGTextureHandle FCascadeShadowMap::AddPasses(FRGBuilder& Graph, FSceneProxy* Scene)
{
	const FCSMSettings Settings = GRenderOptions.GetCSMSettings();
	FRGTextureDesc Desc = FRGTextureDesc::Create2DArray(Resolution, Resolution, CascadeCount, PF_ShadowDepth, ETextureCreateFlags::DepthStencilTarget | ETextureCreateFlags::ShaderResource); Desc.SetClearColor(FRHITextureClearColor(0.0f, 0));
	FRGTextureHandle ShadowTexture = Graph.CreateTexture(Desc, FRGName("DirectionalCSM"));
	for (uint32 Cascade = 0; Cascade < CascadeCount; ++Cascade)
	{
		struct FPassParameters { FRGTextureHandle Shadow; }; const uint32 CascadeIndex = Cascade;
		const FPassParameters& Pass = Graph.AddPass<FPassParameters>(FRGName(std::string("CascadeShadow") + std::to_string(Cascade)), ERGPassFlags::Raster,
			[&](FRGPassBuilder& Builder, FPassParameters& Parameters) { Parameters.Shadow = Builder.UseDepthStencil(ShadowTexture, RTL_Clear, RTS_Store, false, { ERHITextureAspect::Depth, 0, 1, uint16(CascadeIndex), 1 }); },
			[this, Scene, CascadeIndex, Settings](FRHICommandList& Cmd, const FPassParameters&) { Cmd.SetViewport(0, 0, Resolution, Resolution); if (!bActive) return; Cmd.BindProgram(ShadowProgram.get()); Cmd.BindUniformBuffer(UL::BShadow, PassUniformBuffers[CascadeIndex].get()); Cmd.SetGraphicsPipelineState(MakeShadowPipeline(Settings.DepthBias, Settings.SlopeBias)); Scene->DrawSceneDepth(Cmd); Cmd.BindProgram(nullptr); });
		ShadowTexture = Pass.Shadow;
	}
	return ShadowTexture;
}

void FCascadeShadowMap::BindLighting(FRHICommandList& Cmd, FRHITexture* ShadowTexture)
{
	Cmd.BindUniformBuffer(UL::BShadow, LightingUniformBuffer.get()); Cmd.BindTexture(UL::SDirectionalShadow, ShadowTexture, ComparisonSampler.get());
}
}
