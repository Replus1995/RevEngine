#include "Rev/Render/RenderProxy/StaticMeshProxy.h"
#include "Rev/Render/Mesh/StaticMesh.h"
#include "Rev/Render/Material/SurfaceMaterial.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/Renderer/Renderer.h"
#include "Rev/Render/UniformLayout.h"

#include "Rev/World/Entity.h"


namespace Rev
{

FStaticMeshRenderData::FStaticMeshRenderData(const Ref<StaticMesh>& InStaticMesh, const Math::FMatrix4& InModelMatrix)
	: MeshData(InStaticMesh)
	, ModelParams({ InModelMatrix })
{
}

FStaticMeshProxy::FStaticMeshProxy()
{
}

FStaticMeshProxy::~FStaticMeshProxy()
{
}

void FStaticMeshProxy::Prepare(const Ref<FScene>& Scene)
{
	// Todo:
		// update data only if dirty
		// sort all static meshes by depth
		// do frustum culling
	
	auto EntityView = Scene->EntityView<StaticMeshComponent, TransformComponent>();
	for (const auto& [Entiny, MeshComp, TransComp] : EntityView.each())
	{
		mRenderDataArr.emplace_back(MeshComp.StaticMesh, TransComp.Transform.ToMatrix());
	}

}

void FStaticMeshProxy::Cleanup()
{
	mRenderDataArr.clear();
}

void FStaticMeshProxy::FreeResource()
{
}

void FStaticMeshProxy::DrawMeshes(const FRenderer* Renderer, const Math::FMatrix4& ViewProjMat, EMaterialBlendMode InBlend, bool bUseMeshMaterial)
{
	for (auto& RenderData : mRenderDataArr)
	{
		RenderData.ModelParams.MVPMat = ViewProjMat * RenderData.ModelParams.ModelMat;
		Renderer->GetBuiltInUB()->UpdateSubData(&RenderData.ModelParams, sizeof(FModelUniform), REV_MODEL_UNIFORM_OFFSET);
		DrawPrimitives(RenderData, InBlend, bUseMeshMaterial);
	}
}

void FStaticMeshProxy::DrawPrimitives(const FStaticMeshRenderData& InData, EMaterialBlendMode InBlend, bool bUseMeshMaterial) const
{
	for (uint32 i = 0; i < InData.MeshData->GetMaterialCount(); i++)
	{
		auto& pMat = InData.MeshData->GetMaterial(i);
		if (!pMat || pMat->BlendMode != InBlend)
			continue;

		auto vPrimitives = InData.MeshData->GetPrimitive(i);
		if (!vPrimitives.empty())
		{
			if (bUseMeshMaterial)
			{
				pMat->PreDraw();
			}

			for (auto& pPrimitive : vPrimitives)
			{
				RenderCmd::DrawPrimitive(pPrimitive);
			}

			if (bUseMeshMaterial)
			{
				pMat->PostDraw();
			}
		}
	}
}


}