#include "Rev/Render/Renderer.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/Resource/VertexBuffer.h"
#include "Rev/Render/Resource/Shader.h"
#include "Rev/Render/Mesh/StaticMesh.h"
#include "Rev/Render/Material/Material.h"

namespace Rev
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

void Renderer::Clear(const Math::FVector4& color)
{
	RenderCmd::SetClearColor(color);
	RenderCmd::Clear();
}

void Renderer::DrawIndexed(const class VertexArray* vertexArray)
{
	RenderCmd::DrawIndexed(vertexArray);
}

}