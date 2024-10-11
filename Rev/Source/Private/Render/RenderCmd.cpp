#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/RHI/RHICore.h"
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
	FRHICore::Init();
	sContext = FRHICore::CreateContext();
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
	FRHICore::Cleanup();
}

void RenderCmd::Flush()
{
	sContext->Flush();
}

void RenderCmd::BeginFrame(bool bClearBackBuffer)
{
	sContext->BeginFrame(bClearBackBuffer);
}

void RenderCmd::EndFrame()
{
	sContext->EndFrame();
}

void RenderCmd::PresentFrame()
{
	sContext->PresentFrame();
}

void RenderCmd::SetVSync(bool bEnable)
{
	sContext->SetVSync(bEnable);
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

void RenderCmd::BeginRenderPass(const Ref<FRHIRenderPass>& InRenderPass)
{
	sContext->BeginRenderPass(InRenderPass);
}

void RenderCmd::EndRenderPass()
{
	sContext->EndRenderPass();
}

void RenderCmd::NextSubpass()
{
	sContext->NextSubpass();
}

void RenderCmd::BindUniformBuffer(const Ref<FRHIUniformBuffer>& InBuffer, uint16 InBinding)
{
	sContext->BindUniformBuffer(InBuffer, InBinding);
}

void RenderCmd::BindTexture(const Ref<FRHITexture>& InTexture, uint16 InBinding)
{
	sContext->BindTexture(InTexture, InBinding);
}

void RenderCmd::BindProgram(const Ref<FRHIShaderProgram>& InProgram)
{
	sContext->BindProgram(InProgram);
}

void RenderCmd::DrawPrimitive(const Ref<FRHIPrimitive>& InPrimitive)
{
	sContext->DrawPrimitive(InPrimitive);
}

FRHIContext* RenderCmd::GetContext()
{
	return sContext.get();
}

}