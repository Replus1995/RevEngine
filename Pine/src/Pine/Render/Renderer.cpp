#include "pinepch.h"
#include "Pine/Render/Renderer.h"
#include "Pine/Render/RenderCmd.h"
#include "Pine/Render/Resource/VertexBuffer.h"
#include "Pine/Render/Resource/Shader.h"
#include "Pine/Render/StaticMesh.h"
#include "Pine/Render/Material.h"

namespace Pine
{

static ShaderLibrary* sShaderLib = nullptr;

void Renderer::Init()
{
	sShaderLib = new ShaderLibrary;
	RenderCmd::Init();
}

void Renderer::Shutdown()
{
	RenderCmd::Shutdown();
	delete sShaderLib;
	sShaderLib = nullptr;
}

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
	RenderCmd::SetViewport(0, 0, width, height);
}

ShaderLibrary* Renderer::GetShaderLibrary()
{
	return sShaderLib;
}

void Renderer::Clear(const glm::vec4& color)
{
	RenderCmd::SetClearColor(color);
	RenderCmd::Clear();
}

void Renderer::DrawIndexed(const class VertexArray* vertexArray)
{
	RenderCmd::DrawIndexed(vertexArray);
}

}