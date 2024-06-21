#include "pinepch.h"
#include "StaticMeshRenderProxy.h"
#include "Pine/Render/StaticMesh.h"
#include "Pine/Render/Material.h"
#include "Pine/Render/RenderProxy/SceneRenderProxy.h"
#include "Pine/Render/RenderCmd.h"
#include "Pine/Render/Renderer.h"

namespace Pine
{

void StaticMeshRenderProxy::Prepare(const Ref<StaticMesh>& mesh, const glm::mat4& transform)
{
	mStaticMesh = mesh;
	mTransform = transform;
}

void StaticMeshRenderProxy::Draw(EMaterialDomain domain)
{
	for (uint32 i = 0; i < mStaticMesh->GetMaterialCount(); i++)
	{
		auto& pMat = mStaticMesh->GetMaterial(i);
		if(!pMat || pMat->GetDomain() != domain)
			continue;

		auto pVertexArrs = mStaticMesh->GetVertexArrayByIndex(i);
		if (!pVertexArrs.empty())
		{
			pMat->Bind();

			for (auto& pVertexArr : pVertexArrs)
			{
				RenderCmd::DrawIndexed(pVertexArr);
			}

			pMat->Unbind();
		}
	}
}

}