#include "pinepch.h"
#include "Pine/Render/Renderer.h"
#include "Pine/Render/RenderCmd.h"

namespace Pine
{

void Renderer::Init()
{
	RenderCmd::Init();
}

void Renderer::Shutdown()
{
	RenderCmd::Shutdown();
}

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
}

void Renderer::Draw(RenderView* view)
{
}

}