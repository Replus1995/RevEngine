#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHIShader.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Render/Mesh/MeshPrimitive.h"
#include "Rev/Core/Assert.h"


namespace Rev
{

static Scope<FRHIContext> sContext;

void RenderCmd::Init()
{
	REV_CORE_ASSERT(!sContext, "RenderCmd already initialized!");
	sContext = FRHIContext::Create();
	sContext->Init();
	FRHIShaderLibrary::CreateInstance();

	SetClearColor(Math::FLinearColor(0, 0, 0, 1));
}

void RenderCmd::Shutdown()
{
	FRHIShaderLibrary::GetInstance().ClearShadersCache();
	FRHIShaderLibrary::ReleaseInstance();
	sContext->Cleanup();
	sContext.reset();
}

void RenderCmd::BeginFrame()
{
	sContext->BeginFrame();
}

void RenderCmd::EndFrame()
{
	sContext->EndFrame();
}

void RenderCmd::SetViewport(uint32 x, uint32 y, uint32 width, uint32 height)
{
	sContext->SetViewport(x, y, width, height);
}

void RenderCmd::SetClearColor(const Math::FLinearColor& color)
{
	sContext->SetClearColor(color);
}

void RenderCmd::ClearBackBuffer()
{
	sContext->ClearBackBuffer();
}

void RenderCmd::EnableDepthTest(bool bEnable)
{
	sContext->EnableDepthTest(bEnable);
}

void RenderCmd::EnableDepthWrite(bool bEnable)
{
	sContext->EnableDepthWrite(bEnable);
}

void RenderCmd::SetDepthTestMode(EDepthTestMode InMode)
{
	sContext->SetDepthTestMode(InMode);
}

void RenderCmd::SetBlendMode(EBlendMode InMode)
{
	sContext->SetBlendMode(InMode);
}

void RenderCmd::SetCullFaceMode(ECullFaceMode InMode)
{
	sContext->SetCullFaceMode(InMode);
}

void RenderCmd::BindProgram(const Ref<FRHIShaderProgram>& InProgram)
{
	sContext->Bind(InProgram);
}

void RenderCmd::BindUniformBuffer(const Ref<FRHIUniformBuffer>& InUniformBuffer, uint32 InUnit)
{
	sContext->Bind(InUniformBuffer, InUnit);
}

void RenderCmd::BindTexture(const Ref<FRHITexture>& InTexture, uint32 InUnit)
{
	sContext->Bind(InTexture, InUnit);
}

void RenderCmd::BindRenderTarget(const Ref<FRHIRenderTarget>& InRenderTarget)
{
	sContext->Bind(InRenderTarget);
}

void RenderCmd::DrawPrimitive(const FMeshPrimitive* InPrimitive)
{
	if(!InPrimitive)
		return;
	sContext->Draw(InPrimitive->VertexData, InPrimitive->DrawMode);
}

void RenderCmd::Draw(const Ref<FRHIVertexArray>& InVertexArray, EDrawMode InDrawMode)
{
	sContext->Draw(InVertexArray, InDrawMode);
}

}