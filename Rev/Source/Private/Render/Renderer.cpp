#include "Rev/Render/Renderer.h"
#include "Rev/Render/RenderCmd.h"

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