#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/BaseRHI.h"
#include "Rev/Core/Assert.h"

namespace Rev
{
Scope<BaseRHI> RenderCmd::sRHI;

void RenderCmd::Init()
{
	RE_CORE_ASSERT(!sRHI, "RenderCmd already initialized!");
	sRHI = BaseRHI::Create();
	sRHI->Init();
}

void RenderCmd::Shutdown()
{
	sRHI.reset();
}

void RenderCmd::SetViewport(uint32 x, uint32 y, uint32 width, uint32 height)
{
	sRHI->SetViewport(x, y, width, height);
}

void RenderCmd::SetClearColor(const Math::FLinearColor& color)
{
	sRHI->SetClearColor(color);
}

void RenderCmd::Clear()
{
	sRHI->Clear();
}

void RenderCmd::DrawIndexed(const VertexArray* vertexArray, uint32_t indexCount)
{
	sRHI->DrawIndexed(vertexArray, indexCount);
}

}