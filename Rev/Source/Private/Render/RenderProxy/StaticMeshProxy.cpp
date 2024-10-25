#include "Rev/Render/RenderProxy/StaticMeshProxy.h"
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
		if(MeshComp.StaticMesh->IsEmpty())
			continue;

		if (MeshCount < mRenderDataArr.size())
		{
			FStaticMeshRenderData& RenderData = mRenderDataArr[MeshCount];
			RenderData.StaticMesh = MeshComp.StaticMesh;
			RenderData.MeshParams.ModelMat = TransComp.Transform.ToMatrix();
			RenderData.MeshUB->UpdateSubData(&RenderData.MeshParams, sizeof(FStaticMeshUniform));
		}
		else
		{
			FStaticMeshRenderData RenderData;
			RenderData.StaticMesh = MeshComp.StaticMesh;
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
		mRenderDataArr[i].StaticMesh = nullptr;
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
	RHICmdList.GetContext()->BindUniformBuffer(UL::BStaticMesh, InData.MeshUB.get());

	InData.StaticMesh->PrepareDraw();
	RHICmdList.GetContext()->RHISetVertexStream(0, InData.StaticMesh->GetVertexBuffer().GetPositionBufferRHI());
	RHICmdList.GetContext()->RHISetVertexStream(1, InData.StaticMesh->GetVertexBuffer().GetNormalBufferRHI());
	RHICmdList.GetContext()->RHISetVertexStream(2, InData.StaticMesh->GetVertexBuffer().GetTangentBufferRHI());
	RHICmdList.GetContext()->RHISetVertexStream(3, InData.StaticMesh->GetVertexBuffer().GetTexCoordBufferRHI());
	RHICmdList.GetContext()->RHISetVertexStream(4, InData.StaticMesh->GetVertexBuffer().GetColorBufferRHI());

	for (uint32 i = 0; i < InData.StaticMesh->GetNumMaterials(); i++)
	{
		auto pMat = InData.StaticMesh->GetMaterial(i);
		if (bUseMeshMaterial)
		{
			if (!pMat || pMat->BlendMode != InBlend)
				continue;
		}

		auto Sections = InData.StaticMesh->GetSectionsForMaterial(i);
		if (!Sections.empty())
		{
			if (bUseMeshMaterial)
			{
				pMat->PreDraw(RHICmdList);
			}

			for (auto Section : Sections)
			{
				FRHIBuffer* IndexBufferRHI = InData.StaticMesh->GetIndexBuffer().GetIndexBufferRHI();
				RHICmdList.GetContext()->RHIDrawPrimitiveIndexed(IndexBufferRHI, Section->NumTriangles, Section->StartIndex);
			}

			if (bUseMeshMaterial)
			{
				pMat->PostDraw(RHICmdList);
			}

		}
	}
}


}