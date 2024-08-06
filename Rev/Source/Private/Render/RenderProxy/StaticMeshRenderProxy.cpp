#include "Rev/Render/RenderProxy/StaticMeshRenderProxy.h"
#include "Rev/Render/Mesh/StaticMesh.h"
#include "Rev/Render/Material/SurfaceMaterial.h"
#include "Rev/Render/RenderProxy/SceneRenderProxy.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/Renderer.h"
#include "Rev/Render/UniformLayout.h"


namespace Rev
{

StaticMeshRenderProxy::StaticMeshRenderProxy()
{
}

StaticMeshRenderProxy::~StaticMeshRenderProxy()
{
}

void StaticMeshRenderProxy::Prepare(const Ref<StaticMesh>& InMesh, const Math::FTransform& InTransform)
{
	mStaticMesh = InMesh;
	mModelMat = InTransform.ToMatrix();
}

void StaticMeshRenderProxy::DrawPrimitives(EBlendMode InBlend, const Ref<FRHIShaderProgram>& InProgram) const
{
	bool bUseProgramOverride = InProgram != nullptr;
	for (uint32 i = 0; i < mStaticMesh->GetMaterialCount(); i++)
	{
		auto& pMat = mStaticMesh->GetMaterial(i);
		if (!pMat || pMat->BlendMode != InBlend)
			continue;

		auto vPrimitives = mStaticMesh->GetPrimitive(i);
		if (!vPrimitives.empty())
		{
			if (!bUseProgramOverride)
			{
				pMat->PreDraw();
			}

			for (auto& pPrimitive : vPrimitives)
			{
				RenderCmd::DrawPrimitive(pPrimitive);
			}

			if (!bUseProgramOverride)
			{
				pMat->PostDraw();
			}
		}
	}
}

}