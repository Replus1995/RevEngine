#include "Rev/Render/Renderer.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/RHI/RHIVertexBuffer.h"
#include "Rev/Render/Mesh/StaticMesh.h"
#include "Rev/Render/Material/Material.h"

namespace Rev
{

Math::FLinearColor Renderer::sClearColor = Math::FLinearColor(0,0,0,1);

void Renderer::Init()
{
	RenderCmd::Init();
}

void Renderer::Shutdown()
{
	RenderCmd::Shutdown();
}

void Renderer::ResizeWindow(uint32 InWidth, uint32 InHeight)
{
	RenderCmd::SetViewport(0, 0, InWidth, InHeight);
}

}