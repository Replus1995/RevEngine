#include "Rev/Render/RenderProxy/StaticMeshProxy.h"
#include "Rev/Render/Mesh/StaticMesh.h"
#include "Rev/Render/Material/SurfaceMaterial.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/Renderer/Renderer.h"
#include "Rev/Render/UniformLayout.h"

#include "Rev/World/Entity.h"


namespace Rev
{

FStaticMeshProxy::FStaticMeshProxy()
{
}

FStaticMeshProxy::~FStaticMeshProxy()
{
}

void FStaticMeshProxy::Prepare(const Ref<FScene>& Scene, FRenderer* Renderer)
{
	// Todo:
		// update data only if dirty
		// sort all static meshes by depth
		// do frustum culling
	
	auto EntityView = Scene->EntityView<StaticMeshComponent, TransformComponent>();
	for (const auto& [Entiny, MeshComp, TransComp] : EntityView.each())
	{
		FStaticMeshRenderData RenderData;
		RenderData.MeshData = MeshComp.StaticMesh;
		RenderData.ModelParams.ModelMat = TransComp.Transform.ToMatrix();
		RenderData.DynamicOffset = Renderer->GetModelUB()->UpdateSubData(&RenderData.ModelParams, sizeof(FStaticMeshUniform));
		mRenderDataArr.emplace_back(std::move(RenderData));
	}

}

void FStaticMeshProxy::Cleanup()
{
	mRenderDataArr.clear();
}

void FStaticMeshProxy::FreeResource()
{
}

void FStaticMeshProxy::DrawMeshes(const FRenderer* Renderer, EMaterialBlendMode InBlend, bool bUseMeshMaterial)
{
	for (auto& RenderData : mRenderDataArr)
	{
		DrawPrimitives(RenderData, Renderer, InBlend, bUseMeshMaterial);
	}
}

void FStaticMeshProxy::DrawPrimitives(const FStaticMeshRenderData& InData, const FRenderer* Renderer, EMaterialBlendMode InBlend, bool bUseMeshMaterial) const
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
				RenderCmd::DrawPrimitive(pPrimitive->PrimitiveData, pMat->GetProgram(), Renderer->GetModelUB(), InData.DynamicOffset);
			}

			if (bUseMeshMaterial)
			{
				pMat->PostDraw();
			}
		}
	}
}


}