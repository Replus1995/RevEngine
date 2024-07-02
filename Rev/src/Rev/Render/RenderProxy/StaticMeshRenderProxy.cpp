#include "StaticMeshRenderProxy.h"
#include "Rev/Render/StaticMesh.h"
#include "Rev/Render/Material.h"
#include "Rev/Render/RenderProxy/SceneRenderProxy.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/Renderer.h"

namespace Rev
{

void StaticMeshRenderProxy::Prepare(const Ref<StaticMesh>& mesh, const Math::FMatrix4& transform)
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
			pMat->uTransform = mTransform;
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