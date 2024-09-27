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

void RenderCmd::DrawPrimitive(const Ref<FRHIPrimitive>& InPrimitive, const Ref<FRHIShaderProgram>& InProgram, const Ref<FRHIUniformBufferDynamic>& InUniformBufferDynamic, uint32 InDynamicOffset)
{
	sContext->DrawPrimitive(InPrimitive, InProgram, InUniformBufferDynamic, InDynamicOffset);
}

FRHIContext* RenderCmd::GetContext()
{
	return sContext.get();
}

}