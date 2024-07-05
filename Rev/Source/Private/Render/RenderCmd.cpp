#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHIShader.h"
#include "Rev/Render/Mesh/MeshPrimitive.h"
#include "Rev/Core/Assert.h"


namespace Rev
{
Scope<FRHIContext> RenderCmd::sContext;
Scope<FRHIShaderLibrary> sShaderLib;

void RenderCmd::Init()
{
	RE_CORE_ASSERT(!sContext, "RenderCmd already initialized!");
	sShaderLib = CreateScope<FRHIShaderLibrary>();
	sContext = FRHIContext::Create();
	sContext->Init();
	
}

void RenderCmd::Shutdown()
{
	sContext.reset();
	sShaderLib.reset();
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

void RenderCmd::DrawPrimitive(const FMeshPrimitive* pPrimitive)
{
	if (pPrimitive->Valid())
	{
		sContext->DrawIndexed(pPrimitive->VertexData, 0);
	}
}

FRHIShaderLibrary* RenderCmd::GetShaderLibrary()
{
	return sShaderLib.get();
}

}