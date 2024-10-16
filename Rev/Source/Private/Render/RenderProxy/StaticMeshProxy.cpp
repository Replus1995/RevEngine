#include "Rev/Render/RenderProxy/StaticMeshProxy.h"
#include "Rev/Render/Mesh/StaticMesh.h"
#include "Rev/Render/Material/SurfaceMaterial.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHICommandList.h"
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

void FStaticMeshProxy::Prepare(const Ref<FScene>& Scene)
{
	// Todo:
		// sort all static meshes by depth
		// do frustum culling
	
	uint32 MeshCount = 0;
	auto EntityView = Scene->EntityView<StaticMeshComponent, TransformComponent>();
	for (const auto& [Entiny, MeshComp, TransComp] : EntityView.each())
	{
		if (MeshCount < mRenderDataArr.size())
		{
			FStaticMeshRenderData& RenderData = mRenderDataArr[MeshCount];
			RenderData.MeshData = MeshComp.StaticMesh;
			RenderData.MeshParams.ModelMat = TransComp.Transform.ToMatrix();
			RenderData.MeshUB->UpdateSubData(&RenderData.MeshParams, sizeof(FStaticMeshUniform));
		}
		else
		{
			FStaticMeshRenderData RenderData;
			RenderData.MeshData = MeshComp.StaticMesh;
			RenderData.MeshParams.ModelMat = TransComp.Transform.ToMatrix();
			RenderData.MeshUB = GDynamicRHI->CreateUniformBuffer(sizeof(FStaticMeshUniform));
			RenderData.MeshUB->UpdateSubData(&RenderData.MeshParams, sizeof(FStaticMeshUniform));
			mRenderDataArr.emplace_back(std::move(RenderData));
		}
		MeshCount++;
	}

}

void FStaticMeshProxy::Cleanup()
{
	for (size_t i = 0; i < mRenderDataArr.size(); i++)
	{
		mRenderDataArr[i].MeshData = nullptr;
	}
}

void FStaticMeshProxy::FreeResource()
{
}

void FStaticMeshProxy::DrawMeshes(FRHICommandList& RHICmdList, EMaterialBlendMode InBlend, bool bUseMeshMaterial)
{
	for (auto& RenderData : mRenderDataArr)
	{
		DrawPrimitives(RHICmdList, RenderData, InBlend, bUseMeshMaterial);
	}
}

void FStaticMeshProxy::DrawPrimitives(FRHICommandList& RHICmdList, const FStaticMeshRenderData& InData, EMaterialBlendMode InBlend, bool bUseMeshMaterial) const
{
	RHICmdList.GetContext()->BindUniformBuffer(InData.MeshUB, UL::BStaticMesh);
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
				pMat->PreDraw(RHICmdList);
			}

			for (auto& pPrimitive : vPrimitives)
			{
				RHICmdList.GetContext()->DrawPrimitive(pPrimitive->PrimitiveData);
			}

			if (bUseMeshMaterial)
			{
				pMat->PostDraw(RHICmdList);
			}
		}
	}
}


}