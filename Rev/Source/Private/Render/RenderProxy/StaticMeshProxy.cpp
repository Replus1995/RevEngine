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

void FStaticMeshProxy::DrawMeshesDepth(FRHICommandList& RHICmdList)
{
	for (auto& RenderData : mRenderDataArr)
	{
		PrepareMeshDraw(RHICmdList, RenderData);
		for (uint32 i = 0; i < RenderData.StaticMesh->GetNumSections(); i++)
		{
			auto Section = RenderData.StaticMesh->GetSection(i);
			FRHIBuffer* IndexBufferRHI = RenderData.StaticMesh->GetIndexBuffer().GetIndexBufferRHI();
			RHICmdList.GetContext()->RHIDrawPrimitiveIndexed(IndexBufferRHI, Section->NumTriangles, Section->StartIndex);
		}
	}
}

void FStaticMeshProxy::DrawMeshesOpaque(FRHICommandList& RHICmdList)
{
	for (auto& RenderData : mRenderDataArr)
	{
		PrepareMeshDraw(RHICmdList, RenderData);
		for (uint32 i = 0; i < RenderData.StaticMesh->GetNumMaterials(); i++)
		{
			auto pMat = RenderData.StaticMesh->GetMaterial(i);
			if (!pMat || (pMat->BlendMode != MBM_Opaque && pMat->BlendMode != MBM_Masked))
				continue;

			auto Sections = RenderData.StaticMesh->GetSectionsForMaterial(i);
			if (!Sections.empty())
			{
				pMat->PreDraw(RHICmdList);
				for (auto Section : Sections)
				{
					FRHIBuffer* IndexBufferRHI = RenderData.StaticMesh->GetIndexBuffer().GetIndexBufferRHI();
					RHICmdList.GetContext()->RHIDrawPrimitiveIndexed(IndexBufferRHI, Section->NumTriangles, Section->StartIndex);
				}
				pMat->PostDraw(RHICmdList);

			}
		}
	}
}


void FStaticMeshProxy::PrepareMeshDraw(FRHICommandList& RHICmdList, const FStaticMeshRenderData& InData)
{
	RHICmdList.GetContext()->BindUniformBuffer(UL::BStaticMesh, InData.MeshUB.get());
	InData.StaticMesh->GetVertexBuffer().UpdateVertexStreams(RHICmdList);
}


}