#include "pinepch.h"
#include "Pine/Render/Renderer.h"
#include "Pine/Render/RenderCmd.h"
#include "Pine/Render/VertexBuffer.h"
#include "Pine/Render/Shader.h"

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
	RenderCmd::SetViewport(0, 0, width, height);
}

void Renderer::Submit(const Ref<class Shader>& shader, const Ref<class VertexArray>& vertexArray, const glm::mat4& transform)
{
}



}