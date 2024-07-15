#include "Rev/Render/RenderProxy/StaticMeshRenderProxy.h"
#include "Rev/Render/Mesh/StaticMesh.h"
#include "Rev/Render/RenderProxy/SceneRenderProxy.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/Renderer.h"

namespace Rev
{

StaticMeshRenderProxy::StaticMeshRenderProxy()
{
}

StaticMeshRenderProxy::~StaticMeshRenderProxy()
{
}

void StaticMeshRenderProxy::Prepare(const Ref<StaticMesh>& mesh, const Math::FMatrix4& transform)
{
	mStaticMesh = mesh;
	mTransformProxy = transform;
}

void StaticMeshRenderProxy::Draw(EMaterialDomain domain)
{
	for (uint32 i = 0; i < mStaticMesh->GetMaterialCount(); i++)
	{
		auto& pMat = mStaticMesh->GetMaterial(i);
		if(!pMat || pMat->Domain != domain)
			continue;

		auto vPrimitives = mStaticMesh->GetPrimitive(i);
		if (!vPrimitives.empty())
		{
			RenderCmd::PrepareMaterial(pMat.get());
			pMat->Bind();
			mTransformProxy.Upload(pMat->GetProgram());

			for (auto& pPrimitive : vPrimitives)
			{
				RenderCmd::DrawPrimitive(pPrimitive);
			}

			pMat->Unbind();
		}
	}
}

}