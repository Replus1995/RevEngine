#include "Rev/Render/RenderProxy/StaticMeshRenderProxy.h"
#include "Rev/Render/Mesh/StaticMesh.h"
#include "Rev/Render/RenderProxy/SceneRenderProxy.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/Renderer.h"

//Private
#include "Render/UniformLocation.h"

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

void StaticMeshRenderProxy::DrawPrimitives(EMaterialDomain InDomain, EBlendMode InBlend, const Ref<FRHIShaderProgram>& InProgram)
{
	bool bUseProgramOverride = InProgram != nullptr;
	for (uint32 i = 0; i < mStaticMesh->GetMaterialCount(); i++)
	{
		auto& pMat = mStaticMesh->GetMaterial(i);
		if (!pMat || pMat->TessEval != InDomain || pMat->BlendMode != InBlend)
			continue;

		auto vPrimitives = mStaticMesh->GetPrimitive(i);
		if (!vPrimitives.empty())
		{
			if (!bUseProgramOverride)
			{
				RenderCmd::PrepareMaterial(pMat.get());
				pMat->GetProgram()->Bind();
				pMat->SyncUniform();
				//pMat->GetProgram()->SetUniform(UL_ModelMat, mModelMat);
			}
			/*else
			{
				InProgram->SetUniform(UL_ModelMat, mModelMat);
			}*/

			for (auto& pPrimitive : vPrimitives)
			{
				RenderCmd::DrawPrimitive(pPrimitive);
			}

			if (!bUseProgramOverride)
			{
				pMat->GetProgram()->Unbind();
			}
		}
	}
}

}