#include "pinepch.h"
#include "Pine/Render/RenderCmd.h"
#include "Pine/Render/BaseRHI.h"
#include "Pine/Core/Assert.h"

namespace Pine
{
Scope<BaseRHI> RenderCmd::sRHI;

void RenderCmd::Init()
{
	PE_CORE_ASSERT(!sRHI, "RenderCmd already initialized!");
	sRHI = BaseRHI::Create();
	sRHI->Init();
}

void RenderCmd::Shutdown()
{
	sRHI.reset();
}

void RenderCmd::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	sRHI->SetViewport(x, y, width, height);
}

void RenderCmd::SetClearColor(const FVector4& color)
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