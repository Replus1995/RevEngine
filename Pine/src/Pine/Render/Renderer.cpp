#include "pinepch.h"
#include "Pine/Render/Renderer.h"
#include "Pine/Render/RenderCmd.h"
#include "Pine/Render/Resource/VertexBuffer.h"
#include "Pine/Render/Resource/Shader.h"
#include "Pine/Render/Mesh.h"
#include "Pine/Render/Material.h"

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

void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
{
	shader->Bind();
	RenderCmd::DrawIndexed(vertexArray);
}

void Renderer::DrawStaticMesh(const Ref<StaticMesh>& mesh, const glm::mat4& transform)
{
	auto& pUnitMeshes = mesh->GetUnitMeshes();
	for (auto& pUnitMesh : pUnitMeshes)
	{
		Material* pMat = mesh->GetMaterial(pUnitMesh->mMaterialIndex);
		pMat->Bind();
		RenderCmd::DrawIndexed(pUnitMesh->mVertexData);
	}
}



}