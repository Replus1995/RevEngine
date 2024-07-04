#include "Rev/Render/Renderer.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/Resource/VertexBuffer.h"
#include "Rev/Render/Resource/Shader.h"
#include "Rev/Render/Mesh/StaticMesh.h"
#include "Rev/Render/Material/Material.h"

namespace Rev
{

static ShaderLibrary* sShaderLib = nullptr;
Math::FLinearColor Renderer::sClearColor = Math::FLinearColor(0,0,0,1);

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

void Renderer::ResizeWindow(uint32 InWidth, uint32 InHeight)
{
	RenderCmd::SetViewport(0, 0, InWidth, InHeight);
}

ShaderLibrary* Renderer::GetShaderLibrary()
{
	return sShaderLib;
}


}