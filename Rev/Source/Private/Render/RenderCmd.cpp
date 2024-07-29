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
	RE_CORE_ASSERT(!sContext, "RenderCmd already initialized!");
	sContext = FRHIContext::Create();
	sContext->Init();
	FRHIShaderLibrary::CreateInstance();
	
}

void RenderCmd::Shutdown()
{
	FRHIShaderLibrary::GetInstance().ClearShadersCache();
	FRHIShaderLibrary::ReleaseInstance();
	sContext.reset();
}

void RenderCmd::SetViewport(uint32 x, uint32 y, uint32 width, uint32 height)
{
	sContext->SetViewport(x, y, width, height);
}

void RenderCmd::SetClearColor(const Math::FLinearColor& color)
{
	sContext->SetClearColor(color);
}

void RenderCmd::Clear()
{
	sContext->Clear();
}

void RenderCmd::PrepareMaterial(const Material* InMaterial)
{
	sContext->PrepareMaterial(InMaterial);
}

void RenderCmd::DrawPrimitive(const FMeshPrimitive* InPrimitive)
{
	if(!InPrimitive)
		return;
	sContext->DrawVertices(InPrimitive->VertexData, InPrimitive->DrawMode);
}

void RenderCmd::DrawVertices(const Ref<FRHIVertexArray>& InVertexArray, EDrawMode InDrawMode)
{
	sContext->DrawVertices(InVertexArray, InDrawMode);
}

}