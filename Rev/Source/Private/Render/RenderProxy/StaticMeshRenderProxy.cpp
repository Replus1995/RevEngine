#include "Rev/Render/RenderProxy/StaticMeshRenderProxy.h"
#include "Rev/Render/Mesh/StaticMesh.h"
#include "Rev/Render/RenderProxy/SceneRenderProxy.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/Renderer.h"

#include "../UniformLocation.hpp"

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
	mModelMat = transform;
}

void StaticMeshRenderProxy::DrawColored(EMaterialDomain InDomain, EBlendMode InBlend)
{
	for (uint32 i = 0; i < mStaticMesh->GetMaterialCount(); i++)
	{
		auto& pMat = mStaticMesh->GetMaterial(i);
		if(!pMat || pMat->Domain != InDomain || pMat->BlendMode != InBlend)
			continue;

		auto vPrimitives = mStaticMesh->GetPrimitive(i);
		if (!vPrimitives.empty())
		{
			RenderCmd::PrepareMaterial(pMat.get());
			pMat->GetProgram()->Bind();
			pMat->GetProgram()->SetUniform(UL_ModelMat, mModelMat);
			pMat->SyncUniform();

			for (auto& pPrimitive : vPrimitives)
			{
				RenderCmd::DrawPrimitive(pPrimitive);
			}

			pMat->GetProgram()->Unbind();
		}
	}
}

void StaticMeshRenderProxy::DrawShadow(EMaterialDomain InDomain, EBlendMode InBlend, const Ref<FRHIShaderProgram>& InProgram)
{
	for (uint32 i = 0; i < mStaticMesh->GetMaterialCount(); i++)
	{
		auto& pMat = mStaticMesh->GetMaterial(i);
		if (!pMat || pMat->Domain != InDomain || pMat->BlendMode != InBlend)
			continue;

		auto vPrimitives = mStaticMesh->GetPrimitive(i);
		if (!vPrimitives.empty())
		{
			InProgram->SetUniform(UL_ModelMat, mModelMat);
			for (auto& pPrimitive : vPrimitives)
			{
				RenderCmd::DrawPrimitive(pPrimitive);
			}
		}
	}
}

}