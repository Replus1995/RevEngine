#include "Rev/Render/RenderProxy/StaticMeshRenderProxy.h"
#include "Rev/Render/Mesh/StaticMesh.h"
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

		auto vPrimitives = mStaticMesh->GetPrimitive(i);
		if (!vPrimitives.empty())
		{
			pMat->uTransform = mTransform;
			pMat->Bind();

			for (auto& pPrimitive : vPrimitives)
			{
				RenderCmd::DrawPrimitive(pPrimitive);
			}

			pMat->Unbind();
		}
	}
}

}